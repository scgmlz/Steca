//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/fastyamlloader.cpp
//! @brief     Implements libyaml encapsulation in namespace loadYAML.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/loaders/fastyamlloader.h"
#include "core/misc/exception.h"
#include <sstream>

// Allows for a very verbious yaml parser for debugging purposes:
#ifdef VERBIOUS_YAML_PARSER
    #define YAML_DEBUG_OUT(a) qDebug() << a;
#else
    #define YAML_DEBUG_OUT(a)
#endif

namespace loadYAML {

YamlNode::SequenceType::iterator YamlNode::begin()
{
    if (nodeType_ != eNodeType::SEQUENCE)
        THROW("unexpected node where we expected sequence begin");
    return sequence_->begin();
}

YamlNode::SequenceType::iterator YamlNode::end()
{
    if (nodeType_ != eNodeType::SEQUENCE)
        THROW("unexpected node where we expected sequence end");
    return sequence_->end();
}

YamlNode::SequenceType::const_iterator YamlNode::begin() const
{
    if (nodeType_ != eNodeType::SEQUENCE)
        THROW("unexpected node where we expected sequence cbegin");
    return sequence_->cbegin();
}

YamlNode::SequenceType::const_iterator YamlNode::end() const
{
    if (nodeType_ != eNodeType::SEQUENCE)
        THROW("unexpected node where we expected sequence cend");
    return sequence_->cend();
}

yaml_event_type_t parser_parse(YamlParserType parser, yaml_event_t& event)
{
    if (!yaml_parser_parse(parser.get(), &event))
       THROW(QString::fromStdString("Parser error " + std::to_string(parser->error)));
    return event.type;
}

YamlNode parseYamlFast(YamlParserType parser, const yaml_event_t& prevEvent)
{
    switch(prevEvent.type) {
    case YAML_NO_EVENT:
        THROW("DEBUG[parseYamlFast2] YAML_NO_EVENT");
        break;
    case YAML_STREAM_START_EVENT: {
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_STREAM_START_EVENT");
        yaml_event_t event;
        parser_parse(parser, event);
        return parseYamlFast(parser, event);
    }
    case YAML_STREAM_END_EVENT:
        THROW("DEBUG[parseYamlFast2] YAML_STREAM_END_EVENT");
        break;
    case YAML_DOCUMENT_START_EVENT: {
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_DOCUMENT_START_EVENT");
        yaml_event_t event;
        parser_parse(parser, event);
        return parseYamlFast(parser, event);
    }
    case YAML_DOCUMENT_END_EVENT:
        THROW("DEBUG[parseYamlFast2] YAML_DOCUMENT_END_EVENT");
        break;
    case YAML_SEQUENCE_START_EVENT: {
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SEQUENCE_START_EVENT");
        YamlNode node = YamlNode(new YamlNode::SequenceType());
        YamlNode::SequenceType& sequence = node.getSequence();

        yaml_event_t event;
        while(YAML_SEQUENCE_END_EVENT != parser_parse(parser, event)) {
            sequence.push_back(parseYamlFast(parser, event));
            yaml_event_delete(&event);
        };
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SEQUENCE_END_EVENT");

        yaml_event_delete(&event);
        return node;
    }
    case YAML_SEQUENCE_END_EVENT:
        THROW("DEBUG[parseYamlFast2] YAML_SEQUENCE_END_EVENT -- BAD");
        break;
    case YAML_MAPPING_START_EVENT: {
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_MAPPING_START_EVENT");
        YamlNode node = YamlNode(new YamlNode::MapType());
        YamlNode::MapType& map = node.getMap();

        yaml_event_t event;
        while(YAML_MAPPING_END_EVENT != parser_parse(parser, event)) {
            QString key = QString::fromLatin1(reinterpret_cast<char*>(event.data.scalar.value));
            YAML_DEBUG_OUT("DEBUG[parseYamlFast2] key == " << key);
            yaml_event_delete(&event);
            parser_parse(parser, event);
            map.insert(key, parseYamlFast(parser, event));
            yaml_event_delete(&event);
        }
        yaml_event_delete(&event);
        return node;
    }
    case YAML_MAPPING_END_EVENT:
        THROW("DEBUG[parseYamlFast2] YAML_MAPPING_END_EVENT");
        break;
    case YAML_ALIAS_EVENT:
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_ALIAS_EVENT");
        THROW(QString("Got alias (anchor %s)"));
        break;
    case YAML_SCALAR_EVENT:
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SCALAR_EVENT = " << QString::fromLatin1(reinterpret_cast<char*>(prevEvent.data.scalar.value)));
        if (reinterpret_cast<char*>(prevEvent.data.scalar.tag) // handle !array2d tag:
                && std::string(reinterpret_cast<char*>(prevEvent.data.scalar.tag)) == "!array2d") {
            YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SCALAR_EVENT, tag = !array2d");

            std::stringstream arrayStr(reinterpret_cast<char*>(prevEvent.data.scalar.value), std::ios_base::in);

            std::shared_ptr<YamlArray2d> array2d(new YamlArray2d);

            arrayStr >> array2d->width;
            arrayStr >> array2d->height;
            // find start of actual array data:
            while ('[' != arrayStr.get())
            { }
            array2d->data.reserve(array2d->width * array2d->height);
            for (size_t i = 0; i < array2d->width * array2d->height; i++) {
                int v;
                arrayStr >> v;
                array2d->data.push_back(v);
            }
            return YamlNode(array2d);
        }
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SCALAR_EVENT = " << QString::fromLatin1(reinterpret_cast<char*>(prevEvent.data.scalar.value)));
        return YamlNode(QString::fromLatin1(reinterpret_cast<char*>(prevEvent.data.scalar.value)));
    default:
        THROW("unexpected node in parseYamlFast");
    }
}

const YamlNode loadYamlFast(const std::string& filePath) {
    FILEContainer file(fopen(filePath.c_str(), "r"));
    YamlParserType parser( new yaml_parser_t());

    // Initialize parse
    if(!yaml_parser_initialize(&*parser))
        THROW("Failed to initialize parser!");
    if(*file == nullptr)
        THROW("Failed to open file!");

    // Set input file
    yaml_parser_set_input_file(&*parser, *file);

    yaml_event_t event;
    parser_parse(parser, event);
    auto result = parseYamlFast(parser, event);
    yaml_event_delete(&event);
    yaml_parser_delete(parser.get());
    return result;
}

FILE* FILEContainer::operator *() {
    if (value_ == nullptr)
        THROW("value is nullptr, but should be initialized");
    return value_;
}

} // namespace loadYAML

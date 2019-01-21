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
#include "core/base/exception.h"
#include <sstream>

// Allows for a very verbose yaml parser for debugging purposes:
// #define VERBOSE_YAML_PARSER
#ifdef VERBOSE_YAML_PARSER
    #define YAML_DEBUG_OUT(a) qDebug() << a;
#else
    #define YAML_DEBUG_OUT(a)
#endif

namespace {

yaml_event_type_t parser_parse(yaml_parser_t* parser, yaml_event_t& event)
{
    if (yaml_parser_parse(parser, &event))
        return event.type; // regular exit
    // Throw parser error message:
    switch (parser->error) {
        case YAML_MEMORY_ERROR:
            THROW("Memory error: Not enough memory for parsing");
        case YAML_READER_ERROR:
            if (parser->problem_value != -1) {
                THROW(QString("Reader error: %1: #%2 at %3")
                      .arg(parser->problem)
                      .arg(parser->problem_value)
                      .arg(parser->problem_offset));
            } else {
                THROW(QString("Reader error: %1 at %2")
                      .arg(parser->problem)
                      .arg(parser->problem_offset));
            }
        case YAML_SCANNER_ERROR:
            if (parser->context) {
                THROW(QString("Scanner error: %1 at line %2, column %3, "
                              "%4 at line %5, column %6")
                      .arg(parser->context)
                      .arg(parser->context_mark.line+1)
                      .arg(parser->context_mark.column+1)
                      .arg(parser->problem)
                      .arg(parser->problem_mark.line+1)
                      .arg(parser->problem_mark.column+1));
            } else {
                THROW(QString("Scanner error: %1 at line %2, column %3")
                      .arg(parser->problem)
                      .arg(parser->problem_mark.line+1)
                      .arg(parser->problem_mark.column+1));
            }
        case YAML_PARSER_ERROR:
            if (parser->context) {
                THROW(QString("Parser error: %1 at line %2, column %3, "
                              "%4 at line %5, column %6")
                      .arg(parser->context)
                      .arg(parser->context_mark.line+1)
                      .arg(parser->context_mark.column+1)
                      .arg(parser->problem)
                      .arg(parser->problem_mark.line+1)
                      .arg(parser->problem_mark.column+1));
            } else {
                THROW(QString("Parser error: %1 at line %2, column %3")
                      .arg(parser->problem)
                      .arg(parser->problem_mark.line+1)
                      .arg(parser->problem_mark.column+1));
            }
        default:
            break;
    }
    qFatal("unreachable");
}

} // namespace

//  ***********************************************************************************************

namespace loadYAML {

size_t YamlNode::size() const
{
    if (nodeType_ != eNodeType::SEQUENCE)
        qFatal("invalid call of YamlNode::size()");
    return sequence_->size();
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

YamlNode parseYamlFast(yaml_parser_t* parser, const yaml_event_t& prevEvent)
{
    switch(prevEvent.type) {
    case YAML_NO_EVENT:
        THROW("unexpected YAML_NO_EVENT");
    case YAML_STREAM_START_EVENT: {
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_STREAM_START_EVENT");
        yaml_event_t event;
        parser_parse(parser, event);
        return parseYamlFast(parser, event);
    }
    case YAML_STREAM_END_EVENT:
        THROW("unexpected YAML_STREAM_END_EVENT");
    case YAML_DOCUMENT_START_EVENT: {
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_DOCUMENT_START_EVENT");
        yaml_event_t event;
        parser_parse(parser, event);
        return parseYamlFast(parser, event);
    }
    case YAML_DOCUMENT_END_EVENT:
        THROW("unexpected YAML_DOCUMENT_END_EVENT");
        break;
    case YAML_SEQUENCE_START_EVENT: {
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SEQUENCE_START_EVENT");
        auto* sequence = new YamlNode::SequenceType;
        yaml_event_t event;
        while(YAML_SEQUENCE_END_EVENT != parser_parse(parser, event)) {
            sequence->push_back(parseYamlFast(parser, event));
            yaml_event_delete(&event);
        };
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SEQUENCE_END_EVENT");
        yaml_event_delete(&event);
        return YamlNode{sequence};
    }
    case YAML_SEQUENCE_END_EVENT:
        THROW("unexpected YAML_SEQUENCE_END_EVENT");
    case YAML_MAPPING_START_EVENT: {
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_MAPPING_START_EVENT");
        auto* map = new YamlNode::MapType;
        yaml_event_t event;
        while(YAML_MAPPING_END_EVENT != parser_parse(parser, event)) {
            const QString key = QString::fromLatin1(
                reinterpret_cast<const char*>(event.data.scalar.value));
            YAML_DEBUG_OUT("DEBUG[parseYamlFast2] key == " << key);
            yaml_event_delete(&event);
            parser_parse(parser, event);
            const YamlNode value{parseYamlFast(parser, event)};
            map->emplace(key, std::move(value));
            yaml_event_delete(&event);
        }
        yaml_event_delete(&event);
        return YamlNode{map};
    }
    case YAML_MAPPING_END_EVENT:
        THROW("unexpected YAML_MAPPING_END_EVENT");
    case YAML_ALIAS_EVENT:
        THROW("unexpected YAML_ALIAS_EVENT");
    case YAML_SCALAR_EVENT:
        YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SCALAR_EVENT = "
                       << QString::fromLatin1(
                           reinterpret_cast<char*>(prevEvent.data.scalar.value)));
        if (reinterpret_cast<char*>(prevEvent.data.scalar.tag) // handle !array2d tag:
            && std::string(reinterpret_cast<char*>(prevEvent.data.scalar.tag)) == "!array2d") {
            YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SCALAR_EVENT, tag = !array2d");

            std::stringstream arrayStr(
                reinterpret_cast<char*>(prevEvent.data.scalar.value), std::ios_base::in);

            std::shared_ptr<YamlArray2d> array2d(new YamlArray2d);

            arrayStr >> array2d->width;
            arrayStr >> array2d->height;
            // find start of actual array data:
            while ('[' != arrayStr.get())
                ;
            array2d->data.reserve(array2d->width * array2d->height);
            for (size_t i = 0; i < array2d->width * array2d->height; i++) {
                int v;
                arrayStr >> v;
                array2d->data.push_back(v);
            }
            return YamlNode{array2d};
        } else
            YAML_DEBUG_OUT("DEBUG[parseYamlFast2] YAML_SCALAR_EVENT = "
                           << QString::fromLatin1(
                               reinterpret_cast<char*>(prevEvent.data.scalar.value)));
        return YamlNode{QString::fromLatin1(reinterpret_cast<char*>(prevEvent.data.scalar.value))};
    default:
        break;
    }
    qFatal("unreachable");
}

const YamlNode loadYamlFast(const std::string& filePath) {

    FILE* file{fopen(filePath.c_str(), "r")};
    if(!file)
        THROW("Failed to open file!");

    auto* parser = new yaml_parser_t;
    if(!yaml_parser_initialize(parser))
        qFatal("Failed to initialize YAML parser");

    yaml_parser_set_input_file(parser, file);

    yaml_event_t event;
    parser_parse(parser, event);
    auto result = parseYamlFast(parser, event);
    yaml_event_delete(&event);
    yaml_parser_delete(parser);
    return result;
}

} // namespace loadYAML

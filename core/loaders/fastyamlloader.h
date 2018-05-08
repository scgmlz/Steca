//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/fastyamlloader.h
//! @brief     Defines loader functions in namespace load.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FASTYAMLLOADER_H
#define FASTYAMLLOADER_H

#include "core/raw/rawfile.h"
#include "core/typ/exception.h"
#include<yaml.h>
#include<memory>
#include <QVariant>
#include <QMap>
#include <QStringBuilder>
#include <QDebug>

//! Functions loadRawfile and loadComment, and their dependences.

namespace loadYAML {

#define DEBUG_OUT_TEMP(a) // qDebug() << a;

//typedef Container<yaml_parser_t, void, &yaml_parser_delete> YamlParserType;
typedef std::shared_ptr<yaml_parser_t> YamlParserType;

class YamlNode {
public:
    typedef QString KeyType;
    typedef QMap<KeyType, YamlNode> MapType;
    typedef std::vector<YamlNode> SequenceType;
    typedef QString ScalarType;

    enum class eNodeType {
        MAP,
        SEQUENCE,
        SCALAR,
    };

    YamlNode(MapType&& map) : map_(new MapType(map)), nodeType_(eNodeType::MAP) { }
    YamlNode(const MapType& map) : map_(new MapType(map)), nodeType_(eNodeType::MAP) { }
    YamlNode(MapType* map) : map_(map), nodeType_(eNodeType::MAP) { }

    YamlNode(SequenceType&& sequence) : sequence_(new SequenceType(sequence)), nodeType_(eNodeType::SEQUENCE) { }
    YamlNode(const SequenceType& sequence) : sequence_(new SequenceType(sequence)), nodeType_(eNodeType::SEQUENCE) { }
    YamlNode(SequenceType* sequence) : sequence_(sequence), nodeType_(eNodeType::SEQUENCE) { }

    YamlNode( ScalarType&& scalar) : scalar_(scalar), nodeType_(eNodeType::SCALAR) { }
    YamlNode(std::shared_ptr<std::vector<float>> vec) : vec_(vec), nodeType_(eNodeType::SCALAR) { }


    YamlNode(eNodeType nodeType) : nodeType_(nodeType) { }

    YamlNode(bool _isEnd) : isEnd(_isEnd) { }

    YamlNode() { }

    YamlNode(const YamlNode& other)
        : nodeType_(other.nodeType_),
          map_(other.map_),
          sequence_(other.sequence_),
          scalar_(other.scalar_),
          vec_(other.vec_),
          isEnd(other.isEnd)
    { }

    ~YamlNode() { }

    bool isEnd = false;

    inline bool IsDefined() const { return true; } // for now...

    inline bool IsSequence() const { return nodeType() == eNodeType::SEQUENCE; }
    inline bool IsMap() const { return nodeType() == eNodeType::MAP; }
    inline bool IsScalar() const { return nodeType() == eNodeType::SCALAR; }

    inline const ScalarType& value() const {
        return getScalar();
    }

    inline double doubleValue(double deafault = Q_QNAN) const {
        bool ok = false;
        auto result = getScalar().toDouble(&ok);
        return ok ? result : deafault;
    }

    inline float floatValue(float deafault = Q_QNAN) const {
        bool ok = false;
        auto result = getScalar().toFloat(&ok);
        return ok ? result : deafault;
    }

    inline int intValue(int deafault = 0, int base = 10) const {
        bool ok = false;
        auto result = getScalar().toInt(&ok, base);
        return ok ? result : deafault;
    }

    inline std::shared_ptr<std::vector<float>> floatVectorValue() const {
        return vec_;
    }

    YamlNode& operator= ( const YamlNode& other )
    {
        nodeType_ = other.nodeType_;
        *map_ = *other.map_;
        *sequence_ = *other.sequence_;
        scalar_ = other.scalar_;
        *vec_ = *other.vec_;
        isEnd = other.isEnd;
        return *this;
    }

    inline YamlNode& operator[](const KeyType& key)
    {
        //return getMap().find(key)->second;
        return getMap().find(key).value();
    }

    inline const YamlNode& operator[](const KeyType& key) const
    {
        //return getMap().find(key)->second;
        return getMap().find(key).value();
    }

    inline YamlNode& operator[](const int& index)
    {
        return getSequence().at(index);
    }

    inline const YamlNode& operator[](const int& index) const
    {
        return getSequence().at(index);
    }

    const int size() const {
        return sequence_->size();
    }
    inline SequenceType::iterator begin()
    {
        switch (nodeType_) {
        case eNodeType::MAP:
            THROW("node(map) doesn't have an iterator")
            break;
        case eNodeType::SEQUENCE:
            return sequence_->begin();
            break;
        case eNodeType::SCALAR:
            THROW("node(scalar) doesn't have an iterator")
        }
    }

    inline SequenceType::iterator end()
    {
        switch (nodeType_) {
        case eNodeType::MAP:
            THROW("node(map) doesn't have an iterator")
            break;
        case eNodeType::SEQUENCE:
            return sequence_->end();
            break;
        case eNodeType::SCALAR:
            THROW("node(scalar) doesn't have an iterator")
        }
    }

    inline SequenceType::const_iterator begin() const
    {
        switch (nodeType_) {
        case eNodeType::MAP:
            THROW("node(map) doesn't have an iterator")
            break;
        case eNodeType::SEQUENCE:
            return sequence_->cbegin();
            break;
        case eNodeType::SCALAR:
            THROW("node(scalar) doesn't have an iterator")
        }
    }

    inline SequenceType::const_iterator end() const
    {
        switch (nodeType_) {
        case eNodeType::MAP:
            THROW("node(map) doesn't have an iterator")
            break;
        case eNodeType::SEQUENCE:
            return sequence_->cend();
            break;
        case eNodeType::SCALAR:
            THROW("node(scalar) doesn't have an iterator")
        }
    }

    inline const eNodeType nodeType() const { return nodeType_; }



    friend YamlNode&& parseYamlFast(YamlParserType parser, YamlNode&& node);
    friend YamlNode parseYamlFast(YamlParserType parser, const yaml_event_t& prevEvent);
private:

    eNodeType nodeType_;
    const std::shared_ptr<MapType> map_;// = nullptr;
    const std::shared_ptr<SequenceType> sequence_;// = nullptr;
    ScalarType scalar_ = "";
    const std::shared_ptr<std::vector<float>> vec_;

    inline MapType& getMap() { return *map_; }
    inline const MapType& getMap() const { return *map_; }
    inline SequenceType& getSequence() { return *sequence_; }
    inline const SequenceType& getSequence() const { return *sequence_; }
    inline const ScalarType& getScalar() const { return scalar_; }
};

yaml_event_type_t parser_parse(YamlParserType parser, yaml_event_t& event);
YamlNode&& parseYamlFast(YamlParserType parser, YamlNode&& node);
YamlNode parseYamlFast(YamlParserType parser, const yaml_event_t& prevEvent);

struct FILEContainer {
    //Container() : value_(new T()) { }
    FILEContainer(FILE* file) : value_(file) { }
    ~FILEContainer() { fclose(value_); }
    FILE* operator *() {
        if (value_ == nullptr) {
            THROW("value is nullptr, but should be initialized")
        }
        return value_;
    }
private:
    FILE* value_;
};


const YamlNode loadYamlFast(const std::string& filePath);

} // namespace loadYAML

#endif // FASTYAMLLOADER_H

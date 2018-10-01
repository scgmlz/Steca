//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/fastyamlloader.h
//! @brief     Defines libyaml encapsulation in namespace loadYAML.
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
#include <yaml.h>
#include <QMap>
#include <QDebug>

namespace loadYAML {

/// A helper struct for retrieving an !array2d value from a yaml document.
///
/// YamlArray2d contains the width, height and data of the !array2d.
struct YamlArray2d {
    size_t width = 0, height = 0;
    std::vector<float> data;
};

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

    YamlNode(MapType&& map) :
        nodeType_(eNodeType::MAP),
        map_(new MapType(map))
    { }
    YamlNode(const MapType& map) :
        nodeType_(eNodeType::MAP),
        map_(new MapType(map))
    { }
    YamlNode(MapType* map) :
        nodeType_(eNodeType::MAP),
        map_(map)
    { }

    YamlNode(SequenceType&& sequence) :
        nodeType_(eNodeType::SEQUENCE),
        sequence_(new SequenceType(sequence))
    { }
    YamlNode(const SequenceType& sequence) :
        nodeType_(eNodeType::SEQUENCE),
        sequence_(new SequenceType(sequence))
    { }
    YamlNode(SequenceType* sequence) :
        nodeType_(eNodeType::SEQUENCE),
        sequence_(sequence)
    { }

    YamlNode( ScalarType&& scalar) :
        nodeType_(eNodeType::SCALAR) ,
        scalar_(scalar)
    { }
    YamlNode(std::shared_ptr<YamlArray2d> array2d) :
        nodeType_(eNodeType::SCALAR) ,
        array2d_(array2d)
    { }

    YamlNode(eNodeType nodeType) : nodeType_(nodeType) { }

    YamlNode(bool _isEnd) : isEnd(_isEnd) { }

    YamlNode() { }

    YamlNode(const YamlNode& other)
        : isEnd(other.isEnd),
          nodeType_(other.nodeType_),
          map_(other.map_),
          sequence_(other.sequence_),
          scalar_(other.scalar_),
          array2d_(other.array2d_)
    { }

    ~YamlNode() { }

    bool isEnd = false;

    inline bool IsDefined() const { return true; } // for now...

    inline bool IsSequence() const { return nodeType() == eNodeType::SEQUENCE; }
    inline bool IsMap() const { return nodeType() == eNodeType::MAP; }
    inline bool IsScalar() const { return nodeType() == eNodeType::SCALAR; }

    inline const ScalarType& value() const { return getScalar(); }

    inline double doubleValue(double defaultVal = Q_QNAN) const
    {
        bool ok = false;
        auto result = getScalar().toDouble(&ok);
        return ok ? result : defaultVal;
    }

    inline float floatValue(float defaultVal = float(Q_QNAN)) const
    {
        bool ok = false;
        auto result = getScalar().toFloat(&ok);
        return ok ? result : defaultVal;
    }

    inline int intValue(int defaultVal = 0, int base = 10) const
    {
        bool ok = false;
        auto result = getScalar().toInt(&ok, base);
        return ok ? result : defaultVal;
    }

    inline std::shared_ptr<YamlArray2d> array2dValue() const { return array2d_; }

    YamlNode& operator= (const YamlNode& other)
    {
        nodeType_ = other.nodeType_;
        *map_ = *other.map_;
        *sequence_ = *other.sequence_;
        scalar_ = other.scalar_;
        *array2d_ = *other.array2d_;
        isEnd = other.isEnd;
        return *this;
    }

    inline YamlNode& operator[](const KeyType& key)
    {
        return getMap().find(key).value();
    }

    inline const YamlNode& operator[](const KeyType& key) const
    {
        return getMap().find(key).value();
    }

    inline YamlNode& operator[](const size_t& index)
    {
        return getSequence().at(index);
    }

    inline const YamlNode& operator[](const size_t& index) const
    {
        return getSequence().at(index);
    }

    size_t size() const {
        return sequence_->size();
    }   inline SequenceType::iterator begin();

    SequenceType::iterator end();

    SequenceType::const_iterator begin() const;

    SequenceType::const_iterator end() const;

    inline eNodeType nodeType() const { return nodeType_; }

    friend YamlNode&& parseYamlFast(YamlParserType parser, YamlNode&& node);
    friend YamlNode parseYamlFast(YamlParserType parser, const yaml_event_t& prevEvent);

private:
    eNodeType nodeType_;
    const std::shared_ptr<MapType> map_;// = nullptr;
    const std::shared_ptr<SequenceType> sequence_;// = nullptr;
    ScalarType scalar_ = "";
    const std::shared_ptr<YamlArray2d> array2d_;

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
    FILE* operator *();
private:
    FILE* value_;
};


const YamlNode loadYamlFast(const std::string& filePath);

} // namespace loadYAML

#endif // FASTYAMLLOADER_H

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

#include <yaml.h>
#include <memory>
#include <QMap>
#include <QDebug>

namespace loadYAML {

//! Width, height, and data of an !array2d value, used in our YAML extension.
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

    YamlNode() = delete;

    YamlNode(MapType* map) :
        nodeType_(eNodeType::MAP),
        map_(map)
    {}

    YamlNode(SequenceType* sequence) :
        nodeType_(eNodeType::SEQUENCE),
        sequence_(sequence)
    {}

    YamlNode(ScalarType&& scalar) :
        nodeType_(eNodeType::SCALAR) ,
        scalar_(scalar)
    {}

    YamlNode(std::shared_ptr<YamlArray2d> array2d) :
        nodeType_(eNodeType::SCALAR) ,
        array2d_(array2d)
    {}

    inline bool isSequence() const { return nodeType_ == eNodeType::SEQUENCE; }
    inline bool isMap() const { return nodeType_ == eNodeType::MAP; }
    inline bool isScalar() const { return nodeType_ == eNodeType::SCALAR; }

    inline const ScalarType& value() const { return getScalar(); }

    inline double doubleValue(double defaultVal = Q_QNAN) const
    {
        bool ok = false;
        auto result = getScalar().toDouble(&ok);
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
        return *this;
    }

    inline const YamlNode& operator[](const KeyType& key) const
    {
        return getMap().find(key).value();
    }

    inline const YamlNode& operator[](const size_t& index) const
    {
        return getSequence().at(index);
    }

    size_t size() const { return sequence_->size(); }

    SequenceType::const_iterator begin() const;
    SequenceType::const_iterator end() const;

    friend YamlNode parseYamlFast(YamlParserType parser, const yaml_event_t& prevEvent);

private:
    eNodeType nodeType_;
    const std::shared_ptr<MapType> map_;
    const std::shared_ptr<SequenceType> sequence_;
    ScalarType scalar_;
    const std::shared_ptr<YamlArray2d> array2d_;

    inline const MapType& getMap() const { return *map_; }
    inline const SequenceType& getSequence() const { return *sequence_; }
    inline const ScalarType& getScalar() const { return scalar_; }
};

const YamlNode loadYamlFast(const std::string& filePath);

} // namespace loadYAML

#endif // FASTYAMLLOADER_H

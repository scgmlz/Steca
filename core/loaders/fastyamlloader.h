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
#include <map>
#include <memory>
#include <QDebug>

namespace loadYAML {

//! Width, height, and data of an !array2d value, used in our YAML extension.

struct YamlArray2d {
    size_t width = 0, height = 0;
    std::vector<float> data;
};

//! Data tree, to be initialized from YAML file.

class YamlNode {
public:
    typedef QString KeyType;
    typedef std::map<KeyType, YamlNode> MapType;
    typedef std::vector<YamlNode> SequenceType;
    typedef QString ScalarType;

    enum class eNodeType {
        MAP,
        SEQUENCE,
        SCALAR,
    };

    YamlNode() = delete;

    YamlNode(MapType* map) : nodeType_{eNodeType::MAP}, map_{map} {}

    YamlNode(SequenceType* sequence) : nodeType_{eNodeType::SEQUENCE}, sequence_{sequence} {}

    YamlNode(ScalarType&& scalar) : nodeType_{eNodeType::SCALAR}, scalar_{scalar} {}

    YamlNode(std::shared_ptr<YamlArray2d> array2d)
        : nodeType_{eNodeType::SCALAR} , array2d_{array2d} {}

    YamlNode(const YamlNode&) = default; // needed by map_->emplace
    YamlNode& operator= (const YamlNode&) = delete;

    inline bool isSequence() const { return nodeType_ == eNodeType::SEQUENCE; }
    inline bool isMap() const      { return nodeType_ == eNodeType::MAP; }
    inline bool isScalar() const   { return nodeType_ == eNodeType::SCALAR; }

    inline const ScalarType& value() const { return scalar_; }

    inline double doubleValue(double defaultVal = Q_QNAN) const
    {
        bool ok = false;
        auto result = scalar_.toDouble(&ok);
        return ok ? result : defaultVal;
    }

    inline int intValue(int defaultVal = 0, int base = 10) const
    {
        bool ok = false;
        auto result = scalar_.toInt(&ok, base);
        return ok ? result : defaultVal;
    }

    inline std::shared_ptr<YamlArray2d> array2dValue() const { return array2d_; }

    inline const YamlNode& operator[](const KeyType& key) const
    {
        return map_->at(key);
    }

    inline const YamlNode& operator[](const size_t& index) const
    {
        return sequence_->at(index);
    }

    size_t size() const; //!< Returns sequence size

    SequenceType::const_iterator begin() const;
    SequenceType::const_iterator end() const;

private:
    const eNodeType nodeType_;
    const std::shared_ptr<MapType> map_;
    const std::shared_ptr<SequenceType> sequence_;
    const ScalarType scalar_;
    const std::shared_ptr<YamlArray2d> array2d_;
};

const YamlNode loadYamlFast(const std::string& filePath);

} // namespace loadYAML

#endif // FASTYAMLLOADER_H

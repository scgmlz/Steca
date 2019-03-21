//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/metadata.h
//! @brief     Defines class Metadata
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef METADATA_H
#define METADATA_H

#include <QVariant>
#include "core/base/angles.h"
#include "core/typ/mapped.h"

enum class averageMode {
    AVGE,
    SUM,
    FIRST,
    LAST
};

enum class valueType {
    DEG,
    DOUBLE,
    STRING
};

class MetaDefinition {
public:
    MetaDefinition(const QString& niceName, averageMode avgmode, valueType valtype);

    QString name;
    averageMode mode;
    valueType type;
};

//! The meta data associated with one Measurement.

class Metadata : public Mapped {
public:
    Metadata();
    Metadata(const Metadata&) = delete;
    Metadata(Metadata&&) = default;

    static int numAttributes(bool onlyNum);
    static const QString& attributeTag(int, bool nice);
    static const QStringList& attributeTags(bool nice);
    static std::vector<QVariant> attributeNaNs();
    static int size() { return attributeNaNs().size(); }
    static Metadata computeAverage(const std::vector<const Metadata*>& vec);
    static std::vector<MetaDefinition> metaKeys();

    QString attributeStrValue(int) const;
    QVariant attributeValue(int) const;
    std::vector<QVariant> attributeValues() const;

    static std::vector<MetaDefinition> metaKeys_;
    static int noNumAttr;
};

#endif // METADATA_H

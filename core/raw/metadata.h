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
    MetaDefinition(const QString& name, const QString& niceName, averageMode avgmode, valueType valtype);

    const QString asciiName;
    const QString niceName;
    const averageMode mode;
    const valueType type;
};

//! The meta data associated with one Measurement.

class Metadata : public Mapped {
public:
    Metadata();
    Metadata(const Metadata&) = delete;
    Metadata(Metadata&&) = default;

    QString attributeStrValue(int) const;
    QVariant attributeValue(int) const;
    std::vector<QVariant> attributeValues() const;
};

namespace meta {
int numAttributes(bool onlyNum);
const QString& attributeTag(int, bool nice);
const QStringList& attributeTags(bool nice);
std::vector<QVariant> attributeNaNs();
int size();
Metadata computeAverage(const std::vector<const Metadata*>& vec);
} // namespace meta

#endif // METADATA_H

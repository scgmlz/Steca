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

enum class metaMode {
    CONSTANT,
    FILE_DEPENDENT,
    MEASUREMENT_DEPENDENT
};

class MetaDefinition {
public:
    MetaDefinition(const QString& name, const QString& niceName, averageMode avgmode,
                   metaMode mM = metaMode::CONSTANT);

    const QString asciiName_;
    const QString niceName_;
    const averageMode mode_;
    metaMode metaMode_;
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
const QString& asciiTag(int);
const QString& niceTag(int);
const QStringList& asciiTags();
const QStringList& niceTags();
std::vector<QVariant> attributeNaNs();
int size();
Metadata computeAverage(const std::vector<const Metadata*>& vec);
std::vector<QVariant> metaValues(const Mapped metamap);
void setMetaMode(int i, metaMode mM);
metaMode getMetaMode(int);
void clearMetaModes();
int numSelectedFileDependent();
int selectedOfFileDependent(int);
int numSelectedMeasurementDependent();
int selectedOfMeasurementDependent(int);
} // namespace meta

#endif // METADATA_H

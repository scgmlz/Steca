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

#include "core/misc/angles.h"
#include "core/misc/variant.h"

//! The meta data associated with one Measurement.

class Metadata {
public:
    Metadata();
    Metadata(const Metadata&) = delete;
    Metadata(Metadata&&) = default;

    static int numAttributes(bool onlyNum);
    static const QString& attributeTag(int, bool out);
    static const QStringList& attributeTags(bool out);
    static const std::vector<VariantComparator*>& attributeCmps();
    static std::vector<QVariant> attributeNaNs();
    static int size() { return attributeNaNs().size(); }
    static Metadata computeAverage(const std::vector<const Metadata*>& vec);

    QString attributeStrValue(int) const;
    QVariant attributeValue(int) const;
    std::vector<QVariant> attributeValues() const;

    QString date, comment;
    deg motorXT, motorYT, motorZT, motorOmg, motorTth, motorPhi, motorChi, motorPST, motorSST,
        motorOMGM;
    double nmT, nmTeload, nmTepos, nmTeext, nmXe, nmYe, nmZe; // nm = prehistorically 'new' metadata
    double monitorCount, deltaMonitorCount;
    double time, deltaTime;
};

#endif // METADATA_H

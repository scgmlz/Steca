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

#include "core/typ/angles.h"
#include "core/typ/variant.h"
#include <QVector>

//! The meta data associated with one Measurement.

class Metadata {
public:
    Metadata();

    static int numAttributes(bool onlyNum);
    static const QString& attributeTag(int, bool out);
    static const QStringList& attributeTags(bool out);
    static const QVector<VariantComparator*>& attributeCmps();
    static QVector<QVariant> attributeNaNs();
    static int size() { return attributeNaNs().count(); }
    static Metadata computeAverage(const std::vector<const Metadata*>& vec);

    QString attributeStrValue(int) const;
    QVariant attributeValue(int) const;
    QVector<QVariant> attributeValues() const;

    QString date, comment;
    deg motorXT, motorYT, motorZT, motorOmg, motorTth, motorPhi, motorChi, motorPST, motorSST,
        motorOMGM;
    double nmT, nmTeload, nmTepos, nmTeext, nmXe, nmYe, nmZe; // nm = new metadata
    double monitorCount, deltaMonitorCount;
    double time, deltaTime;
};

#endif // METADATA_H

// ************************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/metadata.h
//! @brief     Defines class Metadata
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************************************

#ifndef METADATA_H
#define METADATA_H

#include "core/typ/angles.h"
#include "core/typ/variant.h"
#include <QSharedPointer> // no auto rm
#include <QVector>

//! The meta data associated with one Measurement.

class Metadata {
public:
    Metadata();

    // attribute list - will be dynamic
    static int numAttributes(bool onlyNum);

    static const QString& attributeTag(int, bool out);
    static QStringList attributeTags(bool out);
    static QVector<VariantComparator*> attributeCmps();

    QString attributeStrValue(int) const;
    QVariant attributeValue(int) const;
    QVector<QVariant> attributeValues() const;

    static QVector<QVariant> attributeNaNs();
    static int size() { return attributeNaNs().count(); }

    QString date, comment;

    deg motorXT, motorYT, motorZT, motorOmg, motorTth, motorPhi, motorChi, motorPST, motorSST,
        motorOMGM;

    // new metadata
    double nmT, nmTeload, nmTepos, nmTeext, nmXe, nmYe, nmZe;
    double monitorCount, deltaMonitorCount;
    double time, deltaTime;
};

typedef QSharedPointer<const Metadata> shp_Metadata;

#endif // METADATA_H

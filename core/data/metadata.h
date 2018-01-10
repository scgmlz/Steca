// ************************************************************************** //
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
// ************************************************************************** //

#ifndef METADATA_H
#define METADATA_H

#include "core/typ/angles.h"
#include "core/typ/str.h"
#include "core/typ/variant.h"

class Metadata {
public:
    Metadata();

    // attribute list - will be dynamic
    static uint numAttributes(bool onlyNum);

    static rcstr attributeTag(uint, bool out);
    static QStringList attributeTags(bool out);
    static cmp_vec attributeCmps();

    str attributeStrValue(uint) const;
    QVariant attributeValue(uint) const;
    row_t attributeValues() const;

    static row_t attributeNaNs();

    str date, comment;

    deg motorXT, motorYT, motorZT, motorOmg, motorTth, motorPhi, motorChi, motorPST, motorSST,
        motorOMGM;

    // new metadata
    qreal nmT, nmTeload, nmTepos, nmTeext, nmXe, nmYe, nmZe;
    qreal monitorCount, deltaMonitorCount;
    qreal time, deltaTime;
};

typedef QSharedPointer<const Metadata> shp_Metadata;

#endif // METADATA_H

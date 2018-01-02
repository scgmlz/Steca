// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/metadata.h
//! @brief     Defines class Metadata
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef METADATA_H
#define METADATA_H

#include "typ/angles.h"
#include "typ/str.h"
#include "typ/variant.h"

namespace data {

class Metadata {
public:
    Metadata();

    // attribute list - will be dynamic
    static uint numAttributes(bool onlyNum);

    static rcstr attributeTag(uint, bool out);
    static QStringList attributeTags(bool out);
    static typ::cmp_vec attributeCmps();

    str attributeStrValue(uint) const;
    QVariant attributeValue(uint) const;
    typ::row_t attributeValues() const;

    static typ::row_t attributeNaNs();

    str date, comment;

    typ::deg motorXT, motorYT, motorZT, motorOmg, motorTth, motorPhi, motorChi, motorPST, motorSST,
        motorOMGM;

    // new metadata
    qreal nmT, nmTeload, nmTepos, nmTeext, nmXe, nmYe, nmZe;
    qreal monitorCount, deltaMonitorCount;
    qreal time, deltaTime;
};

} // namespace data

#endif // METADATA_H

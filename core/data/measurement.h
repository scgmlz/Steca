// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/measurement.h
//! @brief     Defines class Measurement
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "core/typ/angles.h"
#include "core/data/image.h"
#include <QSharedPointer> // no auto rm

class Metadata;

//! A Measurement consts of an Image with associated Metadata

class Measurement final {

public:
    Measurement(const Metadata&, const size2d&, const inten_vec&);
    Measurement(const Measurement&) = delete;

    QSharedPointer<Metadata const> metadata() const;

    deg midTth() const;

    qreal monitorCount() const;
    qreal deltaMonitorCount() const;
    qreal deltaTime() const;

    deg omg() const;
    deg phi() const;
    deg chi() const;

    Range rgeGma() const;
    Range rgeGmaFull() const;
    Range rgeTth() const;

    Range rgeInten() const;

    shp_Image image() const { return image_; }
    size2d imageSize() const;

    void collectIntens(const Image* intensCorr, inten_vec&, uint_vec&, const Range&,
                       deg minTth, deg deltaTth) const;

private:
    QSharedPointer<const Metadata> md_;
    shp_Image image_;
};

typedef QSharedPointer<const Measurement> shp_Measurement;

#endif // MEASUREMENT_H

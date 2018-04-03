//  ***********************************************************************************************
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
//  ***********************************************************************************************

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "core/data/image.h"
#include "core/data/metadata.h"
#include <QSharedPointer> // no auto rm
#include <memory>

//! A Measurement consts of an Image with associated Metadata

class Measurement {

public:
    Measurement() = delete;
    Measurement(const Measurement&) = delete;
    Measurement(const int position, const Metadata&, const size2d&, const QVector<float>&);

    int position() const { return position_; }
    const Metadata* metadata() const { return md_.get(); }

    deg midTth() const;

    double monitorCount() const;
    double deltaMonitorCount() const;
    double deltaTime() const;

    deg omg() const;
    deg phi() const;
    deg chi() const;

    Range rgeGma() const;
    Range rgeGmaFull() const;
    Range rgeTth() const;

    Range rgeInten() const;

    const Image& image() const { return *image_; }
    size2d imageSize() const;

    void collectIntens(QVector<float>&, QVector<int>&, const Range&, deg minTth, deg deltaTth) const;

private:
    const int position_; //! position in file_
    std::unique_ptr<Metadata> md_;
    std::unique_ptr<Image> image_;
};

typedef QSharedPointer<const Measurement> shp_Measurement;

#endif // MEASUREMENT_H

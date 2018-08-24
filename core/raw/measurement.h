//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/measurement.h
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

#include "core/raw/image.h"
#include "core/raw/metadata.h"
#include <memory>

//! A Measurement consts of an Image with associated Metadata

class Measurement {

public:
    Measurement() = delete;
    Measurement(const Measurement&) = delete;
    Measurement(Measurement&&) = default;
    Measurement(const int position, Metadata&&, const size2d&, std::vector<float>&&);

    int position() const { return position_; }
    const Metadata& metadata() const { return metadata_; }

    double monitorCount() const;
    double deltaMonitorCount() const;
    double time() const;
    double deltaTime() const;

    deg midTth() const;
    deg omg() const;
    deg phi() const;
    deg chi() const;

    Range rgeInten() const;

    const Image& image() const { return *image_; }
    size2d imageSize() const;

private:
    const int position_; //! position in file_
    Metadata metadata_;
    std::unique_ptr<Image> image_; // TODO consider without pointer
};

#endif // MEASUREMENT_H

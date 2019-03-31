//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/measurement.cpp
//! @brief     Implements class Measurement
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/raw/measurement.h"
//#include "qcr/base/debug.h"

Measurement::Measurement(
    const int position, Metadata&& md, const size2d& size, std::vector<float>&& intens)
    : position_{position}
    , metadata_ {std::move(md)}
    , image_ {new Image{size, std::move(intens)}}
{}

Range Measurement::rgeInten() const { return image_->rgeInten(); }
size2d Measurement::imageSize() const { return image_->size(); }

double Measurement::monitorCount() const { return metadata_.get<double>("mon"); }
double Measurement::deltaMonitorCount() const { return metadata_.get<double>("delta_mon"); }
double Measurement::time() const { return metadata_.get<double>("t"); }
double Measurement::deltaTime() const { return metadata_.get<double>("delta_t"); }

deg Measurement::midTth() const { return metadata_.get<deg>("mid2theta"); }
deg Measurement::omg() const { return metadata_.get<deg>("omega"); }
deg Measurement::phi() const { return metadata_.get<deg>("phi"); }
deg Measurement::chi() const { return metadata_.get<deg>("chi"); }

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

#include "core/session.h"
#include "core/def/idiomatic_for.h"
#include <qmath.h>

Measurement::Measurement(
    const int position, const Metadata& md, const size2d& size, const QVector<float>& intens)
    : position_(position)
    , metadata_(md)
    , image_(new Image(size))
{
    ASSERT(intens.count() == size.count());
    for_i (intens.count())
        image_->setInten(i, intens.at(i));
}

Range Measurement::rgeGma() const
{
    return angleMap().rgeGma();
}

Range Measurement::rgeGmaFull() const
{
    return angleMap().rgeGmaFull();
}

Range Measurement::rgeTth() const
{
    return angleMap().rgeTth();
}

Range Measurement::rgeInten() const
{
    return image_->rgeInten();
}

size2d Measurement::imageSize() const
{
    return image_->size();
}

deg Measurement::midTth() const { return metadata_.motorTth; }

double Measurement::monitorCount() const { return metadata_.monitorCount; }
double Measurement::deltaMonitorCount() const { return metadata_.deltaMonitorCount; }
double Measurement::deltaTime() const { return metadata_.deltaTime; }

deg Measurement::omg() const { return metadata_.motorOmg; }
deg Measurement::phi() const { return metadata_.motorPhi; }
deg Measurement::chi() const { return metadata_.motorChi; }

AngleMap Measurement::angleMap() const
{
    return {ImageKey(gSession->geometry(), gSession->imageSize(), gSession->imageCut(), gSession->midPix(), midTth())};
}

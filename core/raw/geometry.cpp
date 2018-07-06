//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/geometry.cpp
//! @brief     Implements classes Geometry, ImageCut, ScatterDirection, ImageKey
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/def/comparators.h"
#include "core/def/settings.h"
#include "core/session.h"
#include "qcr/base/debug.h"
#include <iostream> // for debugging

#define RET_COMPARE_COMPARABLE(o)            \
    for (int cmp = o.compare(that.o); cmp;)  \
        return cmp;


//  ***********************************************************************************************
//! @class Geometry

// StressSpec standard geometry:
double const Geometry::DEF_DETECTOR_DISTANCE = 1035;
double const Geometry::DEF_DETECTOR_PIXEL_SIZE = 1;

Geometry::Geometry()
{
    // TODO restore constraints?
    // detectorDistance_ = qMin(qMax(detectorDistance, 10.), 9999.);
    // pixSize_ = qMin(qMax(pixSize, .1), 9.9);
}

void Geometry::fromSettings()
{
    XSettings s("DetectorGeometry");
    detectorDistance.setVal(s.readReal("detectorDistance", DEF_DETECTOR_DISTANCE));
    pixSize         .setVal(s.readReal("pixelSize", DEF_DETECTOR_PIXEL_SIZE));
    pixOffset[0]    .setVal(s.readInt("offsetX", 0));
    pixOffset[1]    .setVal(s.readInt("offsetY", 0));
}

void Geometry::toSettings() const
{
    XSettings s("DetectorGeometry");
    s.setValue("detectorDistance", detectorDistance.val());
    s.setValue("pixelSize", pixSize.val());
    s.setValue("offsetX", pixOffset[0].val());
    s.setValue("offsetY", pixOffset[1].val());
}

void Geometry::fromJson(const JsonObj& obj)
{
    detectorDistance.setVal(obj.loadPreal("distance"));
    pixSize.         setVal(obj.loadPreal("pixel size"));
    pixOffset[0]    .setVal(obj.loadPint ("beam offset X"));
    pixOffset[1]    .setVal(obj.loadPint ("beam offset Y"));
}

QJsonObject Geometry::toJson() const
{
    return {
        { "distance", QJsonValue(detectorDistance.val()) },
        { "pixel size", QJsonValue(pixSize.val()) },
        { "beam offset X", QJsonValue(pixOffset[0].val()) },
        { "beam offset Y", QJsonValue(pixOffset[1].val()) }
    };
}

int Geometry::compare(const Geometry& that) const
{
    RET_COMPARE_VALUE(detectorDistance.val())
    RET_COMPARE_VALUE(pixSize.val())
    RET_COMPARE_VALUE(pixOffset[0].val())
    RET_COMPARE_VALUE(pixOffset[1].val())
    return 0;
}

EQ_NE_OPERATOR(Geometry)

//  ***********************************************************************************************
//! @class ImageCut

void ImageCut::clear()
{
    left  .setVal(0);
    right .setVal(0);
    top   .setVal(0);
    bottom.setVal(0);
}

void ImageCut::fromJson(const JsonObj& obj)
{
    left  .setVal(obj.loadUint("left"));
    right .setVal(obj.loadUint("right"));
    top   .setVal(obj.loadUint("top"));
    bottom.setVal(obj.loadUint("bottom"));
}

QJsonObject ImageCut::toJson() const
{
    return {
        { "left", left.val() },
        { "top", top.val() },
        { "right", right.val() },
        { "bottom", bottom.val() },
    };
}

int ImageCut::compare(const ImageCut& that) const
{
    RET_COMPARE_VALUE(left.val())
        RET_COMPARE_VALUE(top.val())
        RET_COMPARE_VALUE(right.val())
        RET_COMPARE_VALUE(bottom.val())
    return 0;
}

EQ_NE_OPERATOR(ImageCut)

size2d ImageCut::marginSize() const
{
    return size2d(horiz(), vertical());
}


//  ***********************************************************************************************
//! @class ScatterDirection

ScatterDirection::ScatterDirection() : ScatterDirection(0, 0) {}

ScatterDirection::ScatterDirection(deg tth_, deg gma_) : tth(tth_), gma(gma_) {}


//  ***********************************************************************************************
//! @class ImageKey

ImageKey::ImageKey(deg midTth_)
    : geometry(gSession->geometry())
    , size(gSession->imageSize())
    , cut(gSession->imageCut())
    , midPixX(size.w/2 + geometry.pixOffset[0].val())
    , midPixY(size.h/2 + geometry.pixOffset[1].val())
    , midTth(midTth_)
{}

int ImageKey::compare(const ImageKey& that) const
{
    RET_COMPARE_COMPARABLE(geometry)
    RET_COMPARE_COMPARABLE(size)
    RET_COMPARE_COMPARABLE(cut)
    RET_COMPARE_VALUE(midPixX)
    RET_COMPARE_VALUE(midPixY)
    RET_COMPARE_VALUE(midTth)
    return 0;
}

EQ_NE_OPERATOR(ImageKey)

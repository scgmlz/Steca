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

#include "core/typ/settings.h"
#include "core/session.h"
#include "qcr/base/debug.h"
#include <iostream> // for debugging

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

    detectorDistance.setHook([](double){ emit gSession->sigDetector(); gRoot->remakeAll("geo"); } );
    pixSize         .setHook([](double){ emit gSession->sigDetector(); gRoot->remakeAll("geo"); } );
    pixOffset[0]    .setHook([](int   ){ emit gSession->sigDetector(); gRoot->remakeAll("geo"); } );
    pixOffset[1]    .setHook([](int   ){ emit gSession->sigDetector(); gRoot->remakeAll("geo"); } );
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


//  ***********************************************************************************************
//! @class ImageCut

ImageCut::ImageCut()
{
    left  .setHook( [](int   ) { emit gSession->sigDetector(); gRoot->remakeAll("cut"); } );
    right .setHook( [](int   ) { emit gSession->sigDetector(); gRoot->remakeAll("cut"); } );
    top   .setHook( [](int   ) { emit gSession->sigDetector(); gRoot->remakeAll("cut"); } );
    bottom.setHook( [](int   ) { emit gSession->sigDetector(); gRoot->remakeAll("cut"); } );
}

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

size2d ImageCut::marginSize() const
{
    return size2d(horiz(), vertical());
}


//  ***********************************************************************************************
//! @class ScatterDirection

ScatterDirection::ScatterDirection() : ScatterDirection(0, 0) {}

ScatterDirection::ScatterDirection(deg tth_, deg gma_) : tth(tth_), gma(gma_) {}

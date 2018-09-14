//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/detector.cpp
//! @brief     Implements classes Detector, ImageCut, ScatterDirection, ImageKey
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/misc/settings.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class Detector

// StressSpec standard detector:
double const Detector::DEF_DETECTOR_DISTANCE = 1035;
double const Detector::DEF_DETECTOR_PIXEL_SIZE = 1;

Detector::Detector()
{
    // from settings:
    XSettings s("DetectorDetector");
    detectorDistance.setVal(s.readReal("detectorDistance", DEF_DETECTOR_DISTANCE));
    pixSize         .setVal(s.readReal("pixelSize", DEF_DETECTOR_PIXEL_SIZE));
    pixOffset[0]    .setVal(s.readInt("offsetX", 0));
    pixOffset[1]    .setVal(s.readInt("offsetY", 0));

    // TODO restore constraints?
    // detectorDistance_ = qMin(qMax(detectorDistance, 10.), 9999.);
    // pixSize_ = qMin(qMax(pixSize, .1), 9.9);

    detectorDistance.setHook([](double){ gSession->onDetector(); });
    pixSize         .setHook([](double){ gSession->onDetector(); });
    pixOffset[0]    .setHook([](int   ){ gSession->onDetector(); });
    pixOffset[1]    .setHook([](int   ){ gSession->onDetector(); });
}

Detector::~Detector()
{
    // to settings
    XSettings s("DetectorDetector");
    s.setValue("detectorDistance", detectorDistance.val());
    s.setValue("pixelSize", pixSize.val());
    s.setValue("offsetX", pixOffset[0].val());
    s.setValue("offsetY", pixOffset[1].val());
}

void Detector::fromJson(const JsonObj& obj)
{
    detectorDistance.setVal(obj.loadPreal("distance"));
    pixSize.         setVal(obj.loadPreal("pixel size"));
    pixOffset[0]    .setVal(obj.loadPint ("beam offset X"));
    pixOffset[1]    .setVal(obj.loadPint ("beam offset Y"));
}

QJsonObject Detector::toJson() const
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
    left  .setHook([this](int val){ sync(val, right, top,   bottom); });
    right .setHook([this](int val){ sync(val, left,  top,   bottom); });
    top   .setHook([this](int val){ sync(val, left,  right, bottom); });
    bottom.setHook([this](int val){ sync(val, left,  right, top   ); });
}

void ImageCut::sync(int val, QcrCell<int>& _1, QcrCell<int>& _2, QcrCell<int>& _3)
{
    if (linked.val()) {
        _1.pureSetVal(val);
        _2.pureSetVal(val);
        _3.pureSetVal(val);
    }
    gSession->onCut();
}

void ImageCut::clear()
{
    left  .pureSetVal(0);
    right .pureSetVal(0);
    top   .pureSetVal(0);
    bottom.pureSetVal(0);
}

void ImageCut::fromJson(const JsonObj& obj)
{
    left  .pureSetVal(obj.loadUint("left"));
    right .pureSetVal(obj.loadUint("right"));
    top   .pureSetVal(obj.loadUint("top"));
    bottom.pureSetVal(obj.loadUint("bottom"));
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

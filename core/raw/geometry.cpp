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

#include "core/session.h"
#include "core/def/comparators.h"
#include "core/def/settings.h"
#include "qcr/engine/debug.h"
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
    detectorDistance.connectAction([](){ emit gSession->sigDetector();});
    pixSize.         connectAction([](){ emit gSession->sigDetector();});
    // TODO restore constraints?
    // detectorDistance_ = qMin(qMax(detectorDistance, 10.), 9999.);
    // pixSize_ = qMin(qMax(pixSize, .1), 9.9);
}

void Geometry::fromSettings()
{
    XSettings s("DetectorGeometry");
    detectorDistance.setParam(s.readReal("detectorDistance", DEF_DETECTOR_DISTANCE));
    pixSize         .setParam(s.readReal("pixelSize", DEF_DETECTOR_PIXEL_SIZE));
    pixOffset[0]    .setParam(s.readInt("offsetX", 0));
    pixOffset[1]    .setParam(s.readInt("offsetY", 0));
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
    detectorDistance.setParam(obj.loadPreal("distance"));
    pixSize.         setParam(obj.loadPreal("pixel size"));
    pixOffset[0]    .setParam(obj.loadPint ("beam offset X"));
    pixOffset[1]    .setParam(obj.loadPint ("beam offset Y"));
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

ImageCut::ImageCut(int left, int top, int right, int bottom)
    : left_(left), top_(top), right_(right), bottom_(bottom)
{
}

void ImageCut::clear()
{
    left_ = 0; top_ = 0; right_ = 0; bottom_ = 0;
}

void ImageCut::fromJson(const JsonObj& obj)
{
    setLeft(obj.loadUint("left"));
    setRight(obj.loadUint("right"));
    setTop(obj.loadUint("top"));
    setBottom(obj.loadUint("bottom"));
    setLinked(obj.loadBool("linked"));
}

QJsonObject ImageCut::toJson() const
{
    return {
        { "left", left_ },
        { "top", top_ },
        { "right", right_ },
        { "bottom", bottom_ },
        { "linked", linked_ }
    };
}

void ImageCut::confine(int& m1, int& m2, int maxTogether)
{
    m1 = qMax(qMin(m1, maxTogether - m2 - 1), 0);
    m2 = qMax(qMin(m2, maxTogether - m1 - 1), 0);
}

void ImageCut::setLeft(int val)
{
    if (linked_) {
        setAll(val);
    } else {
        confine(left_=val, right_, gSession->imageSize().w);
        EMITS("ImageCut::setLeft", gSession->sigDetector());
        // TODO check consequence of rotation implied by imageSize()
    }
}

void ImageCut::setRight(int val)
{
    if (linked_) {
        setAll(val);
    } else {
        confine(right_=val, left_, gSession->imageSize().w);
        EMITS("ImageCut::setRight", gSession->sigDetector());
    }
}

void ImageCut::setTop(int val)
{
    if (linked_) {
        setAll(val);
    } else {
        confine(top_=val, bottom_, gSession->imageSize().h);
        EMITS("ImageCut::setTop", gSession->sigDetector());
    }
}

void ImageCut::setBottom(int val)
{
    if (linked_) {
        setAll(val);
    } else {
        confine(bottom_=val, top_, gSession->imageSize().h);
        EMITS("", gSession->sigDetector());
    }
}

void ImageCut::setLinked(bool val)
{
    linked_ = val;
    EMITS("ImageCut::setLinked", gSession->sigDetector());
}

void ImageCut::setAll(int val)
{
    left_ = right_ = qMax(qMin(val, (gSession->imageSize().w-1)/2), 0);
    top_ = bottom_ = qMax(qMin(val, (gSession->imageSize().h-1)/2), 0);
    EMITS("ImageCut::setAll", gSession->sigDetector());
}

int ImageCut::compare(const ImageCut& that) const
{
    RET_COMPARE_VALUE(left())
        RET_COMPARE_VALUE(top())
        RET_COMPARE_VALUE(right())
        RET_COMPARE_VALUE(bottom())
    return 0;
}

EQ_NE_OPERATOR(ImageCut)

size2d ImageCut::marginSize() const
{
    return size2d(left() + right(), top() + bottom());
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

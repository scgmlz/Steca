// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/geometry.cpp
//! @brief     Implements classes Geometry, ImageCut, ScatterDirection, ImageKey
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/session.h"
#include "core/data/geometry.h"
#include "core/def/comparators.h"
#include "core/def/idiomatic_for.h"
#include <qmath.h>
#include <iostream> // for debugging

#define RET_COMPARE_COMPARABLE(o)                                                 \
    for (int cmp = o.compare(that.o); cmp;)                                       \
        return cmp;


// ************************************************************************** //
//  class Geometry
// ************************************************************************** //

qreal const Geometry::MIN_DETECTOR_DISTANCE = 10;
qreal const Geometry::MIN_DETECTOR_PIXEL_SIZE = .1;

qreal const Geometry::DEF_DETECTOR_DISTANCE = 1035;
qreal const Geometry::DEF_DETECTOR_PIXEL_SIZE = 1;

Geometry::Geometry()
    : detectorDistance(DEF_DETECTOR_DISTANCE), pixSize(DEF_DETECTOR_PIXEL_SIZE), midPixOffset() {}

int Geometry::compare(const Geometry& that) const {
    RET_COMPARE_VALUE(detectorDistance)
    RET_COMPARE_VALUE(pixSize)
    RET_COMPARE_COMPARABLE(midPixOffset)
    return 0;
}

EQ_NE_OPERATOR(Geometry)

// ************************************************************************** //
//  class ImageCut
// ************************************************************************** //

ImageCut::ImageCut(int left, int top, int right, int bottom)
    : _left(left), _top(top), _right(right), _bottom(bottom) {}

void ImageCut::clear() {
    *this = ImageCut();
}

void ImageCut::confine(int& m1, int& m2, int maxTogether) {
    m1 = qMax(qMin(m1, maxTogether - m2 - 1), 0);
    m2 = qMax(qMin(m2, maxTogether - m1 - 1), 0);
}

void ImageCut::setLeft(int val) {
    if (_linked) {
        setAll(val);
    } else {
        confine(_left=val, _right, gSession->imageSize().w);
        emit gSession->sigDetector();
        // TODO check consequence of rotation implied by imageSize()
    }
}

void ImageCut::setRight(int val) {
    if (_linked) {
        setAll(val);
    } else {
        confine(_right=val, _left, gSession->imageSize().w);
        emit gSession->sigDetector();
    }
}

void ImageCut::setTop(int val) {
    if (_linked) {
        setAll(val);
    } else {
        confine(_top=val, _bottom, gSession->imageSize().h);
        emit gSession->sigDetector();
    }
}

void ImageCut::setBottom(int val) {
    if (_linked) {
        setAll(val);
    } else {
        confine(_bottom=val, _top, gSession->imageSize().h);
        emit gSession->sigDetector();
    }
}

void ImageCut::setLinked(bool val) {
    _linked = val;
    emit gSession->sigDetector();
}

void ImageCut::setAll(int val) {
    _left = _right = qMax(qMin(val, (gSession->imageSize().w-1)/2), 0);
    _top = _bottom = qMax(qMin(val, (gSession->imageSize().h-1)/2), 0);
    emit gSession->sigDetector();
}

int ImageCut::compare(const ImageCut& that) const {
    RET_COMPARE_VALUE(left())
        RET_COMPARE_VALUE(top())
        RET_COMPARE_VALUE(right())
        RET_COMPARE_VALUE(bottom())
    return 0;
}

EQ_NE_OPERATOR(ImageCut)

size2d ImageCut::marginSize() const {
    return size2d(left() + right(), top() + bottom());
}


// ************************************************************************** //
//  class ScatterDirection
// ************************************************************************** //

ScatterDirection::ScatterDirection() : ScatterDirection(0, 0) {}

ScatterDirection::ScatterDirection(deg tth_, deg gma_) : tth(tth_), gma(gma_) {}


// ************************************************************************** //
//  class ImageKey
// ************************************************************************** //

ImageKey::ImageKey(
    const Geometry& geometry_, const size2d& size_, const ImageCut& cut_,
    const IJ& midPix_, deg midTth_)
    : geometry(geometry_), size(size_), cut(cut_), midPix(midPix_), midTth(midTth_) {}

int ImageKey::compare(const ImageKey& that) const {
    RET_COMPARE_COMPARABLE(geometry)
    RET_COMPARE_COMPARABLE(size)
    RET_COMPARE_COMPARABLE(cut)
    RET_COMPARE_COMPARABLE(midPix)
    RET_COMPARE_VALUE(midTth)
    return 0;
}

EQ_NE_OPERATOR(ImageKey)

void ImageKey::computeAngles(Array2D<ScatterDirection>& ret) const {
    // detector coordinates: d_x, ... (d_z = const)
    // beam coordinates: b_x, ..; b_y = d_y
    ret.resize(size);
    const qreal t = midTth.toRad();
    const qreal c = cos(t);
    const qreal s = sin(t);
    const qreal d_z = geometry.detectorDistance;
    const qreal b_x1 = d_z * s;
    const qreal b_z1 = d_z * c;
    for_int (i, size.w) {
        const qreal d_x = (i - midPix.i) * geometry.pixSize;
        const qreal b_x = b_x1 + d_x * c;
        const qreal b_z = b_z1 - d_x * s;
        const qreal b_x2 = b_x * b_x;
        for_int (j, size.h) {
            const qreal b_y = (midPix.j - j) * geometry.pixSize; // == d_y
            const qreal b_r = sqrt(b_x2 + b_y * b_y);
            const rad gma = atan2(b_y, b_x);
            const rad tth = atan2(b_r, b_z);
            ret.setAt(i, j, ScatterDirection(tth.toDeg(), gma.toDeg()));
        }
    }
}

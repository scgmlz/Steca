// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/geometry.cpp
//! @brief     Implements classes Geometry, ImageCut, ImageKey
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/data/geometry.h"
#include "core/def/comparators.h"
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

ImageCut::ImageCut() : ImageCut(0, 0, 0, 0) {}

ImageCut::ImageCut(int left_, int top_, int right_, int bottom_)
    : left(left_), top(top_), right(right_), bottom(bottom_) {}

void ImageCut::update(bool topLeftFirst, bool linked, const ImageCut& cut, size2d size) {
    if (size.isEmpty()) {
        *this = ImageCut();
        return;
    }
    auto limit = [linked](int& m1, int& m2, int maxTogether)->void {
        if (linked && m1 + m2 >= maxTogether) {
            m1 = m2 = qMax((maxTogether - 1) / 2, 0);
        } else {
            m1 = qMax(qMin(m1, maxTogether - m2 - 1), 0);
            m2 = qMax(qMin(m2, maxTogether - m1 - 1), 0);
        }
    };

    // make sure that cut values are valid; in the right order
    int _left = cut.left, _top = cut.top, _right = cut.right, _bottom = cut.bottom;

    if (topLeftFirst) {
        limit(_top, _bottom, size.h);
        limit(_left, _right, size.w);
    } else {
        limit(_bottom, _top, size.h);
        limit(_right, _left, size.w);
    }

    *this = ImageCut(_left, _top, _right, _bottom);
}

int ImageCut::compare(const ImageCut& that) const {
    RET_COMPARE_VALUE(left)
    RET_COMPARE_VALUE(top)
    RET_COMPARE_VALUE(right)
    RET_COMPARE_VALUE(bottom)
    return 0;
}

EQ_NE_OPERATOR(ImageCut)

size2d ImageCut::marginSize() const {
    return size2d(left + right, top + bottom);
}

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

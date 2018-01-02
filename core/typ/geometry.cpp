// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/geometry.cpp
//! @brief     Implements classes Geometry, ImageCut
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ/geometry.h"
#include "def/comparators.h"
#include "def/idiomatic_for.h"
#include <qmath.h>
#include <iostream> // for debugging

namespace typ {

preal const Geometry::MIN_DETECTOR_DISTANCE = preal(10);
preal const Geometry::MIN_DETECTOR_PIXEL_SIZE = preal(.1);

preal const Geometry::DEF_DETECTOR_DISTANCE = preal(1035);
preal const Geometry::DEF_DETECTOR_PIXEL_SIZE = preal(1);

Geometry::Geometry()
    : detectorDistance(DEF_DETECTOR_DISTANCE), pixSize(DEF_DETECTOR_PIXEL_SIZE), midPixOffset() {}

int Geometry::compare(Geometry const& that) const {
    RET_COMPARE_VALUE(detectorDistance)
    RET_COMPARE_VALUE(pixSize)
    RET_COMPARE_COMPARABLE(midPixOffset)
    return 0;
}

EQ_NE_OPERATOR(Geometry)

ImageCut::ImageCut() : ImageCut(0, 0, 0, 0) {}

ImageCut::ImageCut(uint left_, uint top_, uint right_, uint bottom_)
    : left(left_), top(top_), right(right_), bottom(bottom_) {}

void ImageCut::update(bool topLeftFirst, bool linked, ImageCut const& cut, typ::size2d size) {
    if (size.isEmpty()) {
        *this = ImageCut();
        return;
    }
    auto limit = [linked](uint& m1, uint& m2, uint maxTogether)->void {
        if (linked && m1 + m2 >= maxTogether) {
            m1 = m2 = qMax((maxTogether - 1) / 2, 0u);
        } else {
            m1 = qMax(qMin(m1, maxTogether - m2 - 1), 0u);
            m2 = qMax(qMin(m2, maxTogether - m1 - 1), 0u);
        }
    };

    // make sure that cut values are valid; in the right order
    uint _left = cut.left, _top = cut.top, _right = cut.right, _bottom = cut.bottom;

    if (topLeftFirst) {
        limit(_top, _bottom, size.h);
        limit(_left, _right, size.w);
    } else {
        limit(_bottom, _top, size.h);
        limit(_right, _left, size.w);
    }

    *this = ImageCut(_left, _top, _right, _bottom);
}

int ImageCut::compare(ImageCut const& that) const {
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

} // namespace typ

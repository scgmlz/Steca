// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_geometry.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ_geometry.h"
#include "def/def_cmp_impl.h"
#include <qmath.h>

namespace typ {

preal const Geometry::MIN_DETECTOR_DISTANCE = preal(10);
preal const Geometry::MIN_DETECTOR_PIXEL_SIZE = preal(.1);

preal const Geometry::DEF_DETECTOR_DISTANCE = preal(1035);
preal const Geometry::DEF_DETECTOR_PIXEL_SIZE = preal(1);

Geometry::Geometry()
    : detectorDistance(DEF_DETECTOR_DISTANCE), pixSize(DEF_DETECTOR_PIXEL_SIZE), midPixOffset() {}

int Geometry::compare(rc that) const {
    RET_COMPARE_VALUE(detectorDistance)
    RET_COMPARE_VALUE(pixSize)
    RET_COMPARE_COMPARABLE(midPixOffset)
    return 0;
}

EQ_NE_OPERATOR(Geometry)

ImageCut::ImageCut() : ImageCut(0, 0, 0, 0) {}

ImageCut::ImageCut(uint left_, uint top_, uint right_, uint bottom_)
    : left(left_), top(top_), right(right_), bottom(bottom_) {}

void ImageCut::update(bool topLeftFirst, bool linked, typ::size2d size) {
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
    uint _left = this->left, _top = this->top, _right = this->right, _bottom = this->bottom;

    if (topLeftFirst) {
        limit(_top, _bottom, size.h);
        limit(_left, _right, size.w);
    } else {
        limit(_bottom, _top, size.h);
        limit(_right, _left, size.w);
    }

    *this = ImageCut(_left, _top, _right, _bottom);
}

int ImageCut::compare(rc that) const {
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

Angles::Angles() : Angles(0, 0) {}

Angles::Angles(deg tth_, deg gma_) : tth(tth_), gma(gma_) {}

AngleMap::Key::Key(
    Geometry::rc geometry_, size2d::rc size_, ImageCut::rc cut_, IJ::rc midPix_, deg midTth_)
    : geometry(geometry_), size(size_), cut(cut_), midPix(midPix_), midTth(midTth_) {}

int AngleMap::Key::compare(rc that) const {
    RET_COMPARE_COMPARABLE(geometry)
    RET_COMPARE_COMPARABLE(size)
    RET_COMPARE_COMPARABLE(cut)
    RET_COMPARE_COMPARABLE(midPix)
    RET_COMPARE_VALUE(midTth)
    return 0;
}

EQ_NE_OPERATOR(AngleMap::Key)

AngleMap::AngleMap(Key::rc key) : key_(key) {
    calculate();
}

static uint lowerBound(vec<deg>::rc vec, deg x, uint i1, uint i2) {
    EXPECT(i1 < i2)

    if (1 == i2 - i1)
        return i1;

    uint mid = (i1 + i2) / 2;
    return vec.at(mid - 1) < x // x may be NaN ...
        ? lowerBound(vec, x, mid, i2)
        : lowerBound(vec, x, i1, mid); // ... we should be so lucky
}

static uint upperBound(vec<deg>::rc vec, deg x, uint i1, uint i2) {
    EXPECT(i1 < i2)

    if (1 == i2 - i1)
        return i2;

    uint mid = (i1 + i2) / 2;
    return vec.at(mid) > x // x may be NaN ...
        ? upperBound(vec, x, i1, mid)
        : upperBound(vec, x, mid, i2); // ... we should be so lucky
}

void AngleMap::getGmaIndexes(
    gma_rge::rc rgeGma, uint_vec const*& indexes, uint& minIndex, uint& maxIndex) const {
    indexes = &gmaIndexes;
    minIndex = lowerBound(gmas, rgeGma.min, 0, gmas.count());
    maxIndex = upperBound(gmas, rgeGma.max, 0, gmas.count());
}

void AngleMap::calculate() {
    auto& geometry = key_.geometry;
    auto& size = key_.size;
    auto& cut = key_.cut;
    auto& midPix = key_.midPix;
    auto& midTth = key_.midTth;

    qreal pixSize = geometry.pixSize, detDist = geometry.detectorDistance;

    arrAngles_.resize(size);

    rgeTth_.invalidate();
    rgeGma_.invalidate();
    rgeGmaFull_.invalidate();

    EXPECT(size.w > cut.left + cut.right)
    EXPECT(size.h > cut.top + cut.bottom)

    uint countWithoutCut = (size.w - cut.left - cut.right) * (size.h - cut.top - cut.bottom);
    EXPECT(countWithoutCut > 0)

    gmas.resize(countWithoutCut);
    gmaIndexes.resize(countWithoutCut);

    // was: adapted from Steca original code
    //  for_int (i, size.w) {
    //    qreal x       = (to_i(i) - midPix.i) * pixSize;
    //    rad   tthHorz = midTth.toRad() + atan(x / detDist);
    //    qreal h       = cos(tthHorz)   * hypot(x, detDist);

    //    for_int (j, size.h) {
    //      qreal y          = (midPix.j - to_i(j)) * pixSize;
    //      qreal z          = hypot(x, y);
    //      qreal pixDetDist = hypot(z, detDist);
    //      rad   tth        = acos(h / pixDetDist);

    //      qreal r     = sqrt((pixDetDist * pixDetDist) - (h * h));
    //      rad   gamma = asin(y / r);

    //      if (tthHorz < 0) {
    //        tth   = -tth;
    //        gamma = -gamma;
    //      }

    //      ENSURE(!qIsNaN(gamma))

    //      arrAngles_.setAt(i, j, Angles(tth.toDeg(), gamma.toDeg()));
    //    }
    //  }

    // new code
    // detector coordinates: d_x, ... (d_z = const)
    // beam coordinates: b_x, ..; b_y = d_y

    qreal d_midTth = midTth.toRad(), cos_midTth = cos(d_midTth), sin_midTth = sin(d_midTth);

    qreal& d_z = detDist;
    qreal b_x1 = d_z * sin_midTth;
    qreal b_z1 = d_z * cos_midTth;

    for_int (i, size.w) {
        qreal d_x = (to_i(i) - midPix.i) * pixSize;

        qreal b_x = b_x1 + d_x * cos_midTth;
        qreal b_z = b_z1 - d_x * sin_midTth;

        qreal b_x2 = b_x * b_x;

        for_int (j, size.h) {
            qreal b_y = (midPix.j - to_i(j)) * pixSize; // == d_y
            qreal b_r = sqrt(b_x2 + b_y * b_y);

            rad gma = atan2(b_y, b_x);
            rad tth = atan2(b_r, b_z);

            arrAngles_.setAt(i, j, Angles(tth.toDeg(), gma.toDeg()));
        }
    }

    uint gi = 0;

    for (uint i = cut.left, iEnd = size.w - cut.right; i < iEnd; ++i) {
        for (uint j = cut.top, jEnd = size.h - cut.bottom; j < jEnd; ++j) {
            auto& as = arrAngles_.at(i, j);

            gmas[gi] = as.gma;
            gmaIndexes[gi] = i + j * size.w;
            ++gi;

            rgeTth_.extendBy(as.tth);
            rgeGmaFull_.extendBy(as.gma);
            if (as.tth >= midTth)
                rgeGma_.extendBy(as.gma); // gma range at mid tth
        }
    }

    uint_vec is(countWithoutCut);
    for_i (is.count())
        is[i] = i;

    std::sort(is.begin(), is.end(), [this](uint i1, uint i2) {
        qreal gma1 = gmas.at(i1), gma2 = gmas.at(i2);
        return gma1 < gma2;
    });

    vec<deg> gv(countWithoutCut);
    for_i (countWithoutCut)
        gv[i] = gmas.at(is.at(i));
    gmas = gv;

    uint_vec uv(countWithoutCut);
    for_i (countWithoutCut)
        uv[i] = gmaIndexes.at(is.at(i));
    gmaIndexes = uv;
}

} // namespace typ

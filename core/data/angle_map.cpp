// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/angle_map.cpp
//! @brief     Implements classes ScatterDirection, AngleMap
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "angle_map.h"
#include "core/def/idiomatic_for.h"
#include <qmath.h>
#include <iostream> // for debugging

namespace {

static int lowerBound(vec<deg> const& vec, deg x, int i1, int i2) {
    debug::ensure(i1 < i2);
    if (1 == i2 - i1)
        return i1;
    int mid = (i1 + i2) / 2;
    return vec.at(mid - 1) < x // x may be NaN ...
        ? lowerBound(vec, x, mid, i2)
        : lowerBound(vec, x, i1, mid); // ... we should be so lucky
}

static int upperBound(vec<deg> const& vec, deg x, int i1, int i2) {
    debug::ensure(i1 < i2);
    if (1 == i2 - i1)
        return i2;
    int mid = (i1 + i2) / 2;
    return vec.at(mid) > x // x may be NaN ...
        ? upperBound(vec, x, i1, mid)
        : upperBound(vec, x, mid, i2); // ... we should be so lucky
}

} // local methods


ScatterDirection::ScatterDirection() : ScatterDirection(0, 0) {}

ScatterDirection::ScatterDirection(deg tth_, deg gma_) : tth(tth_), gma(gma_) {}


AngleMap::AngleMap(ImageKey const& key) : key_(key) { calculate(); }

void AngleMap::getGmaIndexes(
    const Range& rgeGma, vec<int> const*& indexes, int& minIndex, int& maxIndex) const {
    indexes = &gmaIndexes;
    minIndex = lowerBound(gmas, rgeGma.min, 0, gmas.count());
    maxIndex = upperBound(gmas, rgeGma.max, 0, gmas.count());
}

void AngleMap::calculate() {
    const Geometry& geometry = key_.geometry;
    const size2d& size = key_.size;
    const ImageCut& cut = key_.cut;
    const IJ& midPix = key_.midPix;
    const deg midTth = key_.midTth;

    const qreal pixSize = geometry.pixSize, detDist = geometry.detectorDistance;

    arrAngles_.resize(size);

    rgeTth_.invalidate();
    rgeGma_.invalidate();
    rgeGmaFull_.invalidate();

    debug::ensure(size.w > cut.left + cut.right);
    debug::ensure(size.h > cut.top + cut.bottom);

    const int countWithoutCut = (size.w - cut.left - cut.right) * (size.h - cut.top - cut.bottom);
    debug::ensure(countWithoutCut > 0);

    gmas.resize(countWithoutCut);
    gmaIndexes.resize(countWithoutCut);

    // The following is new with respect to Steca1
    // detector coordinates: d_x, ... (d_z = const)
    // beam coordinates: b_x, ..; b_y = d_y

    const qreal d_midTth = midTth.toRad(), cos_midTth = cos(d_midTth), sin_midTth = sin(d_midTth);

    const qreal d_z = detDist;
    const qreal b_x1 = d_z * sin_midTth;
    const qreal b_z1 = d_z * cos_midTth;

    for_int (i, size.w) {
        const qreal d_x = (i - midPix.i) * pixSize;
        const qreal b_x = b_x1 + d_x * cos_midTth;
        const qreal b_z = b_z1 - d_x * sin_midTth;
        const qreal b_x2 = b_x * b_x;
        for_int (j, size.h) {
            const qreal b_y = (midPix.j - j) * pixSize; // == d_y
            const qreal b_r = sqrt(b_x2 + b_y * b_y);
            const rad gma = atan2(b_y, b_x);
            const rad tth = atan2(b_r, b_z);
            arrAngles_.setAt(i, j, ScatterDirection(tth.toDeg(), gma.toDeg()));
        }
    }

    int gi = 0;

    for (int i = cut.left, iEnd = size.w - cut.right; i < iEnd; ++i) {
        for (int j = cut.top, jEnd = size.h - cut.bottom; j < jEnd; ++j) {
            const ScatterDirection& as = arrAngles_.at(i, j);

            gmas[gi] = as.gma;
            gmaIndexes[gi] = i + j * size.w;
            ++gi;

            rgeTth_.extendBy(as.tth);
            rgeGmaFull_.extendBy(as.gma);
            if (as.tth >= midTth)
                rgeGma_.extendBy(as.gma); // gma range at mid tth
        }
    }

    vec<int> is(countWithoutCut);
    for_i (is.count())
        is[i] = i;

    std::sort(is.begin(), is.end(), [this](int i1, int i2) {
        qreal gma1 = gmas.at(i1), gma2 = gmas.at(i2);
        return gma1 < gma2;
    });

    vec<deg> gv(countWithoutCut);
    for_i (countWithoutCut)
        gv[i] = gmas.at(is.at(i));
    gmas = gv;

    vec<int> uv(countWithoutCut);
    for_i (countWithoutCut)
        uv[i] = gmaIndexes.at(is.at(i));
    gmaIndexes = uv;
}

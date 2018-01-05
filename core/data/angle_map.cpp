// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/angle_map.cpp
//! @brief     Implements classes Angles, AngleMap
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "angle_map.h"
#include "def/idiomatic_for.h"
#include <qmath.h>
#include <iostream> // for debugging

AnglePair::AnglePair() : AnglePair(0, 0) {}

AnglePair::AnglePair(deg tth_, deg gma_) : tth(tth_), gma(gma_) {}

AngleMap::AngleMap(ImageKey const& key) : key_(key) {
    calculate();
}

static uint lowerBound(vec<deg> const& vec, deg x, uint i1, uint i2) {
    debug::ensure(i1 < i2);
    if (1 == i2 - i1)
        return i1;
    uint mid = (i1 + i2) / 2;
    return vec.at(mid - 1) < x // x may be NaN ...
        ? lowerBound(vec, x, mid, i2)
        : lowerBound(vec, x, i1, mid); // ... we should be so lucky
}

static uint upperBound(vec<deg> const& vec, deg x, uint i1, uint i2) {
    debug::ensure(i1 < i2);
    if (1 == i2 - i1)
        return i2;
    uint mid = (i1 + i2) / 2;
    return vec.at(mid) > x // x may be NaN ...
        ? upperBound(vec, x, i1, mid)
        : upperBound(vec, x, mid, i2); // ... we should be so lucky
}

void AngleMap::getGmaIndexes(
    Range const& rgeGma, uint_vec const*& indexes, uint& minIndex, uint& maxIndex) const {
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

    debug::ensure(size.w > cut.left + cut.right);
    debug::ensure(size.h > cut.top + cut.bottom);

    uint countWithoutCut = (size.w - cut.left - cut.right) * (size.h - cut.top - cut.bottom);
    debug::ensure(countWithoutCut > 0);

    gmas.resize(countWithoutCut);
    gmaIndexes.resize(countWithoutCut);

    // The following is new with respect to Steca1
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

            arrAngles_.setAt(i, j, AnglePair(tth.toDeg(), gma.toDeg()));
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

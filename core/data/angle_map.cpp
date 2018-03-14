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
#include <iostream> // for debugging

namespace {

//! Returns index i for which v[i-1]<x<=v[i], provided i1<=i<i2.
static int lowerBound(const vec<deg>& vec, deg x, int i1, int i2) {
    ASSERT(i1 < i2);
    if (i2-i1 == 1)
        return i1;
    int mid = (i1+i2) / 2;
    return vec.at(mid - 1) < x // x may be NaN ...
        ? lowerBound(vec, x, mid, i2)
        : lowerBound(vec, x, i1, mid); // ... we should be so lucky
}

//! Returns index i for which v[i-1]<=x<v[i], provided i1<i<=i2.
static int upperBound(const vec<deg>& vec, deg x, int i1, int i2) {
    ASSERT(i1 < i2);
    if (i2-i1 == 1)
        return i2;
    int mid = (i1+i2) / 2;
    return vec.at(mid) > x // x may be NaN ...
        ? upperBound(vec, x, i1, mid)
        : upperBound(vec, x, mid, i2); // ... we should be so lucky
}

} // local methods


AngleMap::AngleMap(const ImageKey& key)
    : key_ {key}
{
    key_.computeAngles(arrAngles_);

    const size2d& size = key_.size;
    const ImageCut& cut = key_.cut;
    ASSERT(size.w > cut.left() + cut.right());
    ASSERT(size.h > cut.top() + cut.bottom());
    const int countWithoutCut =
        (size.w - cut.left() - cut.right()) * (size.h - cut.top() - cut.bottom());
    ASSERT(countWithoutCut > 0);

    // compute ranges rgeTth_, rgeGma_, rgeGmaFull_, and arrays gmas_, gmaIndexes_:
    rgeTth_.invalidate();
    rgeGma_.invalidate();
    rgeGmaFull_.invalidate();
    gmas_.resize(countWithoutCut);
    gmaIndexes_.resize(countWithoutCut);
    int gi = 0;
    for (int i = cut.left(), iEnd = size.w - cut.right(); i < iEnd; ++i) {
        for (int j = cut.top(), jEnd = size.h - cut.bottom(); j < jEnd; ++j) {
            const ScatterDirection& dir = arrAngles_.at(i, j);
            gmas_[gi] = dir.gma;
            gmaIndexes_[gi] = i + j * size.w;
            ++gi;
            rgeTth_.extendBy(dir.tth);
            rgeGmaFull_.extendBy(dir.gma);
            // TODO URGENT: THIS IS WRONG: seems correct only for tth<=90deg
            if (dir.tth >= key_.midTth)
                rgeGma_.extendBy(dir.gma); // gma range at mid tth
        }
    }

    // compute indices of sorted gmas_:
    vec<int> is(countWithoutCut);
    for_i (is.count())
        is[i] = i;
    std::sort(is.begin(), is.end(), [this](int i1, int i2) {
        return gmas_.at(i1) < gmas_.at(i2); });
    // sort gmas_:
    vec<deg> gv(countWithoutCut);
    for_i (countWithoutCut)
        gv[i] = gmas_.at(is.at(i));
    gmas_ = gv;
    // sort gmaIndexes_:
    vec<int> uv(countWithoutCut);
    for_i (countWithoutCut)
        uv[i] = gmaIndexes_.at(is.at(i));
    gmaIndexes_ = uv;
}

void AngleMap::getGmaIndexes(
    const Range& rgeGma, vec<int> const*& indexes, int& minIndex, int& maxIndex) const
{
    indexes = &gmaIndexes_;
    minIndex = lowerBound(gmas_, rgeGma.min, 0, gmas_.count());
    maxIndex = upperBound(gmas_, rgeGma.max, 0, gmas_.count());
}

//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "core/data/angle_map.h"
#include "core/session.h"
#include "qcr/base/debug.h" // ASSERT
#include <qmath.h>
#include <iostream> // for debugging

namespace {

//! Returns index i for which v[i-1]<x<=v[i], provided i1<=i<i2.
static int lowerBound(const std::vector<deg>& vec, deg x, int i1, int i2)
{
    ASSERT(i1 < i2);
    if (i2-i1 == 1)
        return i1;
    int mid = (i1+i2) / 2;
    return vec.at(mid - 1) < x // x may be NaN ...
        ? lowerBound(vec, x, mid, i2)
        : lowerBound(vec, x, i1, mid); // ... we should be so lucky
}

//! Returns index i for which v[i-1]<=x<v[i], provided i1<i<=i2.
static int upperBound(const std::vector<deg>& vec, deg x, int i1, int i2)
{
    ASSERT(i1 < i2);
    if (i2-i1 == 1)
        return i2;
    int mid = (i1+i2) / 2;
    return vec.at(mid) > x // x may be NaN ...
        ? upperBound(vec, x, i1, mid)
        : upperBound(vec, x, mid, i2); // ... we should be so lucky
}

} // local methods

AngleMap::AngleMap(const deg tth)
{
    size_ = gSession->imageSize();
    arrAngles_.resize(size_.count());
    const Detector& geo = gSession->params.detector;
    // compute angles:
    //    detector center is at vec{d} = (d_x, 0, )
    //    detector pixel (i,j) is at vec{b}
    const double t = tth.toRad();
    const double c = cos(t);
    const double s = sin(t);
    const double d_z = geo.detectorDistance.val();
    const double b_x1 = d_z * s;
    const double b_z1 = d_z * c;
    int midPixX = size_.w/2 + geo.pixOffset[0].val();
    int midPixY = size_.h/2 + geo.pixOffset[1].val();
    for (int i=0; i<size_.w; ++i) {
        const double d_x = (i - midPixX) * geo.pixSize.val();
        const double b_x = b_x1 + d_x * c;
        const double b_z = b_z1 - d_x * s;
        const double b_x2 = b_x * b_x;
        for (int j=0; j<size_.h; ++j) {
            const double b_y = (midPixY - j) * geo.pixSize.val(); // == d_y
            const double b_r = sqrt(b_x2 + b_y * b_y);
            const rad gma = atan2(b_y, b_x);
            const rad tth = atan2(b_r, b_z);
            arrAngles_[pointToIndex(i, j)] = {tth.toDeg(), gma.toDeg()};
        }
    }

    const ImageCut& cut = gSession->params.imageCut;
    ASSERT(size_.w > cut.horiz());
    ASSERT(size_.h > cut.vertical());
    const int countAfterCut = (size_.w - cut.horiz()) * (size_.h - cut.vertical());
    ASSERT(countAfterCut > 0);

    // compute ranges rgeTth_, rgeGma_, rgeGmaFull_, and arrays gmas_, gmaIndexes_:
    rgeTth_.invalidate();
    rgeGma_.invalidate();
    rgeGmaFull_.invalidate();
    gmas_.resize(countAfterCut);
    gmaIndexes_.resize(countAfterCut);
    int gi = 0;
    for (int j = cut.top.val(), jEnd = size_.h - cut.bottom.val(); j < jEnd; ++j) {
        for (int i = cut.left.val(), iEnd = size_.w - cut.right.val(); i < iEnd; ++i) {
            // (loop order j-i results in faster stable_sort than order i-j)
            const ScatterDirection& dir = arrAngles_[pointToIndex(i, j)];
            gmas_[gi] = dir.gma;
            gmaIndexes_[gi] = pointToIndex(i, j);
            ++gi;
            rgeTth_.extendBy(dir.tth);
            rgeGmaFull_.extendBy(dir.gma);
            // TODO URGENT: THIS IS WRONG: seems correct only for tth<=90deg
            if (dir.tth >= tth)
                rgeGma_.extendBy(dir.gma); // gma range at mid tth
        }
    }
    //qDebug() << "AngleMap: found gamma range " << rgeGma_.to_s();
    //qDebug() << "AngleMap: found full range " << rgeGmaFull_.to_s();
    //qDebug() << "AngleMap: found theta range " << rgeTth_.to_s();

    // compute indices of sorted gmas_:
    std::vector<int> is(countAfterCut);
    for (int i=0; i<is.size(); ++i)
        is[i] = i;
    //qDebug() << "AngleMap: sort";
    // empirically, stable_sort seems to be faster than sort
    std::stable_sort(is.begin(), is.end(), [this](int i1, int i2) {
            return gmas_.at(i1) < gmas_.at(i2); });
    //qDebug() << "AngleMap: sort/";
    // sort gmas_:
    std::vector<deg> gv(countAfterCut);
    for (int i=0; i<countAfterCut; ++i)
        gv[i] = gmas_.at(is.at(i));
    gmas_ = std::move(gv);
    // sort gmaIndexes_:
    std::vector<int> uv(countAfterCut);
    for (int i=0; i<countAfterCut; ++i)
        uv[i] = gmaIndexes_.at(is.at(i));
    gmaIndexes_ = std::move(uv);
    //qDebug() << "AngleMap/";
}

void AngleMap::getGmaIndexes(
    const Range& rgeGma, std::vector<int> const*& indexes, int& minIndex, int& maxIndex) const
{
    indexes = &gmaIndexes_;
    minIndex = lowerBound(gmas_, rgeGma.min, 0, gmas_.size());
    maxIndex = upperBound(gmas_, rgeGma.max, 0, gmas_.size());
}

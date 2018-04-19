//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/angle_map.cpp
//! @brief     Implements classes ScatterDirection, AngleMap
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "angle_map.h"
#include "core/def/idiomatic_for.h"
#include <cmath>
#include <iostream> // for debugging

namespace {

//! Returns index i for which v[i-1]<x<=v[i], provided i1<=i<i2.
static int lowerBound(const QVector<deg>& vec, deg x, int i1, int i2)
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
static int upperBound(const QVector<deg>& vec, deg x, int i1, int i2)
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


AngleMap::AngleMap(const ImageKey& key)
    : size_(key.size)
    , arrAngles_(key.size.count())
{
    qDebug() << "AngleMap: core computation";
    // compute angles:
    //    detector center is at vec{d} = (d_x, 0, )
    //    detector pixel (i,j) is at vec{b}
    const double t = key.midTth.toRad();
    const double c = cos(t);
    const double s = sin(t);
    const double d_z = key.geometry.detectorDistance();
    const double b_x1 = d_z * s;
    const double b_z1 = d_z * c;
    for_int (i, size_.w) {
        const double d_x = (i - key.midPix.i) * key.geometry.pixSize();
        const double b_x = b_x1 + d_x * c;
        const double b_z = b_z1 - d_x * s;
        const double b_x2 = b_x * b_x;
        for_int (j, size_.h) {
            const double b_y = (key.midPix.j - j) * key.geometry.pixSize(); // == d_y
            const double b_r = sqrt(b_x2 + b_y * b_y);
            const rad gma = atan2(b_y, b_x);
            const rad tth = atan2(b_r, b_z);
            arrAngles_[pointToIndex(i, j)] = ScatterDirection(tth.toDeg(), gma.toDeg());
        }
    }
    qDebug() << "AngleMap: core computation done";

    const ImageCut& cut = key.cut;
    ASSERT(size_.w > cut.left() + cut.right());
    ASSERT(size_.h > cut.top() + cut.bottom());
    const int countWithoutCut =
        (size_.w - cut.left() - cut.right()) * (size_.h - cut.top() - cut.bottom());
    ASSERT(countWithoutCut > 0);

    qDebug() << "AngleMap: compute ranges";
    // compute ranges rgeTth_, rgeGma_, rgeGmaFull_, and arrays gmas_, gmaIndexes_:
    rgeTth_.invalidate();
    rgeGma_.invalidate();
    rgeGmaFull_.invalidate();
    gmas_.resize(countWithoutCut);
    gmaIndexes_.resize(countWithoutCut);
    int gi = 0;
    for (int i = cut.left(), iEnd = size_.w - cut.right(); i < iEnd; ++i) {
        for (int j = cut.top(), jEnd = size_.h - cut.bottom(); j < jEnd; ++j) {
            const ScatterDirection& dir = arrAngles_[pointToIndex(i, j)];
            gmas_[gi] = dir.gma;
            gmaIndexes_[gi] = i + j * size_.w;
            ++gi;
            rgeTth_.extendBy(dir.tth);
            rgeGmaFull_.extendBy(dir.gma);
            // TODO URGENT: THIS IS WRONG: seems correct only for tth<=90deg
            if (dir.tth >= key.midTth)
                rgeGma_.extendBy(dir.gma); // gma range at mid tth
        }
    }
    qDebug() << "AngleMap: found gamma range " << rgeGma_.to_s();
    qDebug() << "AngleMap: found full range " << rgeGmaFull_.to_s();
    qDebug() << "AngleMap: found theta range " << rgeTth_.to_s();

    qDebug() << "AngleMap: compute indices";
    // compute indices of sorted gmas_:
    QVector<int> is(countWithoutCut);
    for_i (is.count())
        is[i] = i;
    std::sort(is.begin(), is.end(), [this](int i1, int i2) {
        return gmas_.at(i1) < gmas_.at(i2); });
    // sort gmas_:
    QVector<deg> gv(countWithoutCut);
    for_i (countWithoutCut)
        gv[i] = gmas_.at(is.at(i));
    gmas_ = gv;
    // sort gmaIndexes_:
    QVector<int> uv(countWithoutCut);
    for_i (countWithoutCut)
        uv[i] = gmaIndexes_.at(is.at(i));
    gmaIndexes_ = uv;
    qDebug() << "AngleMap: compute indices done";
}

void AngleMap::getGmaIndexes(
    const Range& rgeGma, QVector<int> const*& indexes, int& minIndex, int& maxIndex) const
{
    indexes = &gmaIndexes_;
    minIndex = lowerBound(gmas_, rgeGma.min, 0, gmas_.count());
    maxIndex = upperBound(gmas_, rgeGma.max, 0, gmas_.count());
}

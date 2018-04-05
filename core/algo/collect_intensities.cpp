//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/algo/collect_intensities.cpp
//! @brief     Implements function projectIntensity
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "collect_intensities.h"
#include "core/session.h"
#include "core/def/idiomatic_for.h"
#include <qmath.h>

namespace {
//! Increments intens and counts.
void projectIntensity(
    QVector<float>& intens, QVector<int>& counts,
    const Measurement& measurement, const Range& rgeGma, deg minTth, deg deltaTth)
{
    const AngleMap& map = gSession->angleMap(measurement);

    const QVector<int>* gmaIndexes = nullptr;
    int gmaIndexMin = 0, gmaIndexMax = 0;
    map.getGmaIndexes(rgeGma, gmaIndexes, gmaIndexMin, gmaIndexMax);

    ASSERT(gmaIndexes);
    ASSERT(gmaIndexMin <= gmaIndexMax);
    ASSERT(gmaIndexMax <= gmaIndexes->count());

    ASSERT(intens.count() == counts.count());
    int count = intens.count();

    ASSERT(deltaTth > 0);

    for (int i = gmaIndexMin; i < gmaIndexMax; ++i) {
        int ind = gmaIndexes->at(i);
        float inten = measurement.image().inten(ind);
        if (qIsNaN(inten))
            continue;

        float corr
            = gSession->corrset().isActive() ? gSession->corrset().intensCorr()->inten(ind) : 1;
        if (qIsNaN(corr))
            continue;
        inten *= corr;

        // bin index
        deg tth = map.at(ind).tth;
        int ti = qFloor((tth - minTth) / deltaTth);
        ASSERT(ti <= count);
        ti = qMin(ti, count - 1); // it can overshoot due to floating point calculation

        intens[ti] += inten;
        counts[ti] += 1;
    }
}

} // namespace

int algo::numTthBins(const QVector<const Measurement*>& _members, const Range& _rgeTth)
{
    const ImageCut& cut = gSession->imageCut();
    int ret = gSession->imageSize().w - cut.left() - cut.right(); // number of horizontal pixels
    if (_members.size()>1) // for combined cluster, increase ret
        ret = ret * _rgeTth.width() / _members.first()->rgeTth().width();
    ASSERT(ret);
    return ret;
}

Curve algo::collectIntensities(
    const QVector<const Measurement*>& _members,
    double _normFactor, const Range& _rgeGma, const Range& _rgeTth)
{
    int numBins = numTthBins(_members, _rgeTth);
    QVector<float> intens(numBins, 0);
    QVector<int> counts(numBins, 0);

    deg minTth = _rgeTth.min;
    deg deltaTth = _rgeTth.width() / numBins;

    for (const Measurement* one : _members)
        // increment intens and counts
        projectIntensity(intens, counts, *one, _rgeGma, minTth, deltaTth);

    // sum or average
    if (gSession->intenScaledAvg()) {
        double scale = gSession->intenScale();
        for_i (numBins) {
            int cnt = counts.at(i);
            if (cnt > 0)
                intens[i] *= scale / cnt;
        }
    }

    Curve ret;
    for_i (numBins)
        ret.append(minTth + deltaTth * i, double(intens.at(i) * _normFactor));
    return ret;
}

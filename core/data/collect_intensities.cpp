//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/collect_intensities.cpp
//! @brief     Implements function projectIntensity
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/data/collect_intensities.h"
#include "core/session.h"
#include "core/data/from_map.h"
#include "qcr/base/debug.h"
#include <qmath.h>

namespace {

//! Increments intens and counts.
void projectMeasurement(
    std::vector<float>& intens, std::vector<int>& counts,
    const Measurement& measurement, const Range& rgeGma, deg minTth, deg deltaTth)
{
    const AngleMap& angleMap = gSession->angleMap.get(measurement.midTth());

    const std::vector<int>* gmaIndexes = nullptr;
    int gmaIndexMin = 0, gmaIndexMax = 0;
    angleMap.getGmaIndexes(rgeGma, gmaIndexes, gmaIndexMin, gmaIndexMax);

    ASSERT(gmaIndexes);
    ASSERT(gmaIndexMin <= gmaIndexMax);
    ASSERT(gmaIndexMax <= gmaIndexes->size());

    ASSERT(intens.size() == counts.size());
    int count = intens.size();

    ASSERT(deltaTth > 0);

    const Image& normalizer = gSession->corrset.getNormalizer();

    // TODO: MOST TIME IS SPENT HERE => OPTIMIZE !
    for (int i = gmaIndexMin; i < gmaIndexMax; ++i) {
        int ind = gmaIndexes->at(i);
        float inten = measurement.image().inten1d(ind);
        if (qIsNaN(inten))
            continue;

        if (!normalizer.isEmpty()) {
            float corr = normalizer.inten1d(ind);
            if (qIsNaN(corr)) // TODO: correct handling of corr=0
                continue;
            inten *= corr;
        }

        // bin index
        deg tth = angleMap.dirAt1(ind).tth;
        int ti = qFloor((tth - minTth) / deltaTth);
        ASSERT(ti <= count);
        ti = qMin(ti, count - 1); // it can overshoot due to floating point calculation

        intens[ti] += inten;
        counts[ti] += 1;
    }
}

} // namespace


int algo::numTthBins(const std::vector<const Measurement*>& _members, const Range& _rgeTth)
{
    const ImageCut& cut = gSession->params.imageCut;
    int ret = gSession->imageSize().w - cut.horiz(); // number of horizontal pixels
    if (_members.size()>1) // for combined cluster, increase ret
        ret = ret * _rgeTth.width() / fromMap::rgeTth(_members.front()).width();
    ASSERT(ret);
    return ret;
}

//! Computes and returns diffractogram for given image cluster and gamma range.

Curve algo::projectCluster(const Sequence& cluster, const Range& rgeGma)
{
    const std::vector<const Measurement*>& members = cluster.members();
    double normFactor = cluster.normFactor();
    const Range& rgeTth = cluster.rgeTth();

    int numBins = numTthBins(members, rgeTth);
    std::vector<float> intens(numBins, 0);
    std::vector<int> counts(numBins, 0);

    deg minTth = rgeTth.min;
    deg deltaTth = rgeTth.width() / numBins;

    for (const Measurement* one : members)
        // increment intens and counts
        projectMeasurement(intens, counts, *one, rgeGma, minTth, deltaTth);

    // sum or average
    if (gSession->params.intenScaledAvg.val()) {
        double scale = gSession->params.intenScale.val();
        for (int i=0; i<numBins; ++i) {
            int cnt = counts.at(i);
            if (cnt > 0)
                intens[i] *= scale / cnt;
        }
    }

    Curve ret;
    for (int i=0; i<numBins; ++i)
        ret.append(minTth + deltaTth * i, double(intens.at(i) * normFactor));
    return ret;
}

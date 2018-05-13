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
#include "core/raw/angle_map.h"
#include "core/typ/async.h"
#include "qcr/engine/debug.h"
#include <qmath.h>

namespace {
//! Increments intens and counts.
void projectMeasurement(
    std::vector<float>& intens, std::vector<int>& counts,
    const Measurement& measurement, const Range& rgeGma, deg minTth, deg deltaTth)
{
    const AngleMap& angleMap = measurement.angleMap();

    const std::vector<int>* gmaIndexes = nullptr;
    int gmaIndexMin = 0, gmaIndexMax = 0;
    angleMap.getGmaIndexes(rgeGma, gmaIndexes, gmaIndexMin, gmaIndexMax);

    ASSERT(gmaIndexes);
    ASSERT(gmaIndexMin <= gmaIndexMax);
    ASSERT(gmaIndexMax <= gmaIndexes->size());

    ASSERT(intens.size() == counts.size());
    int count = intens.size();

    ASSERT(deltaTth > 0);

    auto* normalizer = gSession->corrset().normalizer();

    // TODO: MOST TIME IS SPENT HERE => OPTIMIZE !
    for (int i = gmaIndexMin; i < gmaIndexMax; ++i) {
        int ind = gmaIndexes->at(i);
        float inten = measurement.image().inten1d(ind);
        if (qIsNaN(inten))
            continue;

        if (normalizer) {
            float corr = normalizer->inten1d(ind);
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
    const ImageCut& cut = gSession->imageCut();
    int ret = gSession->imageSize().w - cut.horiz(); // number of horizontal pixels
    if (_members.size()>1) // for combined cluster, increase ret
        ret = ret * _rgeTth.width() / _members.front()->rgeTth().width();
    ASSERT(ret);
    return ret;
}

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
        ret.append(minTth + deltaTth * i, double(intens.at(i) * normFactor));
    return ret;
}

void algo::projectActiveClusters(class QProgressBar* progressBar)
{
    const ActiveClusters& seq = gSession->activeClusters();
    Progress progress(progressBar, "project intensities", seq.size());
    int nGamma = qMax(1, gSession->gammaSelection().numSlices.val());
    for (const Cluster* cluster : seq.clusters()) {
        progress.step();
        for_i (nGamma) {
            const Range gammaSector = gSession->gammaSelection().slice2range(i);
            cluster->setCurve(i, projectCluster(*cluster, gammaSector));
        }
    }
}

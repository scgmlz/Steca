//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/active_clusters.cpp
//! @brief     Implements class ActiveClusters
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/calc/active_clusters.h"
#include "core/data/collect_intensities.h"
#include "core/aux/async.h"
#include "core/data/lens.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

namespace {

//! Computes average diffractogram.
Curve computeAvgCurve(const ActiveClusters*const ac)
{
    TakesLongTime __("computeAvgCurve");
    // flatten Cluster-Measurement hierarchy into one Sequence
    std::vector<const Measurement*> group;
    for (const Cluster* cluster : ac->clusters.get())
        for (const Measurement* one: cluster->members())
            group.push_back(one);
    const Sequence seq(group);
    // then compute the dfgram
    return algo::projectCluster(seq, seq.rgeGma());
}

Range computeRgeGma(const ActiveClusters*const ac)
{
    Range ret;
    for (const Cluster* cluster : ac->clusters.get())
        ret.extendBy(cluster->rgeGma());
    return ret;
}

Range computeRgeFixedInten(const ActiveClusters*const ac)
{
    bool trans = false; bool cut = false; // TODO restore (broken after d97148958)
    Range ret;
    TakesLongTime __("rgeFixedInten");
    for (const Cluster* cluster : ac->clusters.get())
        for (const Measurement* one : cluster->members())
            ret.extendBy(ImageLens(one->image(), trans, cut).rgeInten(false));
    return ret;
}

//! Internally used to compute grand averages of monitor, time, etc
double recomputeAvg(const ActiveClusters*const ac, std::function<double(const Measurement*)> f)
{
    double sum = 0;
    int cnt = 0;
    for (const Cluster* cluster : ac->clusters.get()) {
        for (const Measurement* one : cluster->members())
            sum += f(one);
        cnt += cluster->size();
    }
    return sum/cnt;
}

} // namespace


ActiveClusters::ActiveClusters()
    : clusters {[]()->std::vector<const Cluster*>{return gSession->dataset.activeClustersList();}}
    , avgDfgram {[this]()->Dfgram{
        return Dfgram(computeAvgCurve(this));}}
    , rgeGma {[this]()->Range{
        return computeRgeGma(this);}}
    , rgeFixedInten {[this]()->Range{
        return computeRgeFixedInten(this);}}
    , grandAvgMonitorCount {[this]()->double{
        return recomputeAvg(this, [](const Measurement* one){return one->monitorCount();});}}
    , grandAvgDeltaMonitorCount {[this]()->double{
        return recomputeAvg(this, [](const Measurement* one){return one->deltaMonitorCount();});}}
    , grandAvgTime {[this]()->double{
        return recomputeAvg(this, [](const Measurement* one){return one->time();});}}
    , grandAvgDeltaTime {[this]()->double{
        return recomputeAvg(this, [](const Measurement* one){return one->deltaTime();});}}
{}

void ActiveClusters::invalidate() const
{
    clusters.invalidate();
    grandAvgMonitorCount.invalidate();
    grandAvgDeltaMonitorCount.invalidate();
    grandAvgTime.invalidate();
    grandAvgDeltaTime.invalidate();
    invalidateAvg();
    gSession->allPeaks.invalidateAll();
}

void ActiveClusters::invalidateAvg() const
{
    avgDfgram.invalidate();
    rgeFixedInten.invalidate();
    rgeGma.invalidate();
}

//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/active_clusters.cpp
//! @brief     Implements class ActiveClusters
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "active_clusters.h"
#include "core/session.h"
#include "core/algo/collect_intensities.h"
#include "core/data/lens.h"
#include "core/data/cluster.h"
#include "core/aux/async.h"
#include "qcr/base/debug.h"

namespace {

//! Computes curve .
Curve computeAvgCurve(const ActiveClusters*const ac)
{
    TakesLongTime __("computeAvgCurve");
    // flatten Cluster-Measurement hierarchy into one Sequence
    std::vector<const Measurement*> group;
    for (const Cluster* cluster : ac->clusters())
        for (const Measurement* one: cluster->members())
            group.push_back(one);
    const Sequence seq(group);
    // then compute the dfgram
    return algo::projectCluster(seq, seq.rgeGma());
}

} // namespace

ActiveClusters::ActiveClusters()
    : avgDfgram( [this]()->Dfgram{ return Dfgram(computeAvgCurve(this)); } )
{
    invalidateAvgMutables();
}

void ActiveClusters::reset(std::vector<std::unique_ptr<Cluster>>& allClusters)
{
    clusters_.clear();
    for (const auto& pCluster : allClusters)
        if (pCluster->isActivated())
            clusters_.push_back(pCluster.get());
    invalidateAvgMutables();
    QObject::connect(gSession, &Session::sigDetector, [=]() { avgDfgram.invalidate(); });
}

double ActiveClusters::recomputeAvg(std::function<double(const Measurement*)> f)
{
    double sum = 0;
    int cnt = 0;
    for (const Cluster* cluster : clusters_) {
        for (const Measurement* one : cluster->members())
            sum += f(one);
        cnt += cluster->count();
    }
    return sum/cnt;
}

const Range& ActiveClusters::rgeGma() const
{
    if (!rgeGma_.isValid())
        for (const Cluster* cluster : clusters_)
            rgeGma_.extendBy(cluster->rgeGma());
    return rgeGma_;
}

const Range& ActiveClusters::rgeFixedInten(bool trans, bool cut) const
{
    if (!rgeFixedInten_.isValid()) {
        TakesLongTime __("rgeFixedInten");
        for (const Cluster* cluster : clusters_)
            for (const Measurement* one : cluster->members())
                rgeFixedInten_.extendBy(ImageLens(one->image(), trans, cut).rgeInten(false));
    }
    return rgeFixedInten_;
}

void ActiveClusters::invalidateAvgMutables() const
{
    avgDfgram.invalidate();
    grandAvgMonitorCount     .invalidate();
    grandAvgDeltaMonitorCount.invalidate();
    grandAvgTime             .invalidate();
    grandAvgDeltaTime        .invalidate();
    rgeFixedInten_.invalidate();
    rgeGma_.invalidate();
}

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
#include "core/calc/lens.h"
#include "core/data/cluster.h"
#include "core/typ/async.h"
#include "qcr/base/debug.h"

ActiveClusters::ActiveClusters()
    : avgCurve(this)
{
    invalidateAvgMutables();
}

void ActiveClusters::reset(std::vector<std::unique_ptr<Cluster>>& allClusters)
{
    clusters_.clear();
    for (const auto& pCluster : allClusters) {
        if (pCluster->isActivated())
            clusters_.push_back(pCluster.get());
    }
    invalidateAvgMutables();
    avgCurve.invalidate();
    QObject::connect(gSession, &Session::sigDetector, [=]() { avgCurve.invalidate(); });
}

#define ACTIVE_AVG(target, what_function)                     \
    if (qIsNaN(avgMonitorCount_)) {                           \
        double sum = 0;                                       \
        int cnt = 0;                                          \
        for (Cluster const* cluster : clusters_) {            \
            for (const Measurement* one : cluster->members()) \
                sum += one->what_function();                  \
            cnt += cluster->count();                          \
        }                                                     \
        target = sum/cnt;                                     \
        qDebug() << "recomputed" << #target << ":" << target; \
    }                                                         \
    return target;

double ActiveClusters::avgMonitorCount() const
{
    ACTIVE_AVG(avgMonitorCount_, monitorCount);
}

double ActiveClusters::avgDeltaMonitorCount() const
{
    ACTIVE_AVG(avgDeltaMonitorCount_, deltaMonitorCount);
}

double ActiveClusters::avgTime() const
{
    ACTIVE_AVG(avgTime_, time);
}

double ActiveClusters::avgDeltaTime() const
{
    ACTIVE_AVG(avgDeltaTime_, deltaTime);
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
    avgMonitorCount_ = avgDeltaMonitorCount_ = avgDeltaTime_ = Q_QNAN;
    rgeFixedInten_.invalidate();
    rgeGma_.invalidate();
    avgCurve_.clear();
}

//! Computes cached avgCurve_.
Curve computeAvgCurve(const ActiveClusters*const ac)
{
    TakesLongTime __("computeAvgCurve");
    // flatten Cluster-Measurement hierarchy into one Sequence
    std::vector<const Measurement*> group;
    for (Cluster const* cluster : ac->clusters())
        for (const Measurement* one: cluster->members())
            group.push_back(one);
    const Sequence seq(group);
    // then compute the dfgram
    return algo::projectCluster(seq, seq.rgeGma());
}

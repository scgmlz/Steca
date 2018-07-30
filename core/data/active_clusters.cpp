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
#include "core/algo/collect_intensities.h"
#include "core/calc/lens.h"
#include "core/data/cluster.h"
#include "core/typ/async.h"
#include "qcr/base/debug.h"

ActiveClusters::ActiveClusters()
{
    invalidateAvgMutables();
}

void ActiveClusters::appendHere(Cluster* cluster)
{
    // can be added only once
    clusters_.push_back(cluster);
    invalidateAvgMutables();
}

#define ACTIVE_AVG(target, what_function)                     \
    if (qIsNaN(avgMonitorCount_)) {                           \
        double sum = 0;                                       \
        int cnt = 0;                                          \
        for (Cluster const* cluster : clusters_) {            \
            double avg = 0;                                   \
            for (const Measurement* one : cluster->members()) \
                avg += one->what_function();                  \
            sum += avg;                                       \
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

//! Returns cached avgCurve_; recomputes if empty.
Curve ActiveClusters::avgCurve() const
{
    if (avgCurve_.isEmpty()) {
        TakesLongTime __("avgCurve");
        computeAvgCurve();
    }
    return avgCurve_;
}

void ActiveClusters::invalidateAvgMutables() const
{
    avgMonitorCount_ = avgDeltaMonitorCount_ = avgDeltaTime_ = Q_QNAN;
    rgeFixedInten_.invalidate();
    rgeGma_.invalidate();
    avgCurve_.clear();
}

//! Computes cached avgCurve_.
void ActiveClusters::computeAvgCurve() const
{
    std::vector<const Measurement*> group;
    for (Cluster const* cluster : clusters_)
        for (const Measurement* one: cluster->members())
            group.push_back(one);
    const Sequence seq(group);
    avgCurve_ = algo::projectCluster(seq, seq.rgeGma());
}

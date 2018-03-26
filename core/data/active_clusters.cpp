// ************************************************************************** //
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
// ************************************************************************** //

#include "core/session.h"

ActiveClusters::ActiveClusters() {
    invalidateAvgMutables();
}

void ActiveClusters::appendHere(const Cluster* cluster) {
    // can be added only once
    clusters_.push_back(cluster);
    invalidateAvgMutables();
}

size2d ActiveClusters::imageSize() const {
    if (clusters_.empty())
        return size2d(0, 0);
    // all images have the same size; simply take the first one
    return clusters_.front()->imageSize();
}

double ActiveClusters::avgMonitorCount() const {
    if (qIsNaN(avgMonitorCount_)) {
        avgMonitorCount_ = calcAvgMutable(&Cluster::avgMonitorCount);
        qDebug() << "recomputed avgMonitorCount: " << avgMonitorCount_;
    }
    return avgMonitorCount_;
}

double ActiveClusters::avgDeltaMonitorCount() const {
    if (qIsNaN(avgDeltaMonitorCount_))
        avgDeltaMonitorCount_ = calcAvgMutable(&Cluster::avgDeltaMonitorCount);
    return avgDeltaMonitorCount_;
}

double ActiveClusters::avgDeltaTime() const {
    if (qIsNaN(avgDeltaTime_))
        avgDeltaTime_ = calcAvgMutable(&Cluster::avgDeltaTime);
    return avgDeltaTime_;
}

const Range& ActiveClusters::rgeGma() const {
    if (!rgeGma_.isValid())
        for (const Cluster* cluster : clusters_)
            rgeGma_.extendBy(cluster->rgeGma());
    return rgeGma_;
}

const Range& ActiveClusters::rgeFixedInten(bool trans, bool cut) const {
    if (!rgeFixedInten_.isValid()) {
        TakesLongTime __;
        for (const Cluster* cluster : clusters_)
            for (const Measurement* one : cluster->members()) {
                if (one->image()) {
                    const shp_Image& image = one->image();
                    rgeFixedInten_.extendBy(ImageLens(*image, trans, cut).rgeInten(false));
                }
            }
    }
    return rgeFixedInten_;
}

Curve ActiveClusters::avgCurve() const {
    if (avgCurve_.isEmpty()) {
        TakesLongTime __;
        computeAvgeCurve();
    }
    return avgCurve_;
}

void ActiveClusters::invalidateAvgMutables() const {
    avgMonitorCount_ = avgDeltaMonitorCount_ = avgDeltaTime_ = Q_QNAN;
    rgeFixedInten_.invalidate();
    rgeGma_.invalidate();
    avgCurve_.clear();
}

//! Computed cached avgeCurve_.
void ActiveClusters::computeAvgeCurve() const {
    QVector<const Measurement*> group;
    for (Cluster const* cluster : clusters_)
        for (const Measurement* one: cluster->members())
            group.append(one);
    avgCurve_ = Sequence(group).toCurve();
}

double ActiveClusters::calcAvgMutable(double (Cluster::*avgFct)() const) const {
    double sum = 0;
    int cnt = 0;
    for (Cluster const* cluster : clusters_) {
        sum += ((*cluster).*avgFct)() * cluster->count();
        cnt += cluster->count();
    }
    return sum/cnt;
}

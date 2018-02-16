// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/experiment.cpp
//! @brief     Implements class Experiment
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/session.h"

Experiment::Experiment() {
    invalidateAvgMutables();
}

void Experiment::appendHere(const Cluster* cluster) {
    // can be added only once
    clusters_.push_back(cluster);
    invalidateAvgMutables();
}

size2d Experiment::imageSize() const {
    if (clusters_.empty())
        return size2d(0, 0);
    // all images have the same size; simply take the first one
    return clusters_.front()->imageSize();
}

qreal Experiment::avgMonitorCount() const {
    if (qIsNaN(avgMonitorCount_)) {
        avgMonitorCount_ = calcAvgMutable(&Cluster::avgMonitorCount);
        qDebug() << "recomputed avgMonitorCount: " << avgMonitorCount_;
    }
    return avgMonitorCount_;
}

qreal Experiment::avgDeltaMonitorCount() const {
    if (qIsNaN(avgDeltaMonitorCount_))
        avgDeltaMonitorCount_ = calcAvgMutable(&Cluster::avgDeltaMonitorCount);
    return avgDeltaMonitorCount_;
}

qreal Experiment::avgDeltaTime() const {
    if (qIsNaN(avgDeltaTime_))
        avgDeltaTime_ = calcAvgMutable(&Cluster::avgDeltaTime);
    return avgDeltaTime_;
}

const Range& Experiment::rgeGma() const {
    if (!rgeGma_.isValid())
        for (const Cluster* cluster : clusters_)
            rgeGma_.extendBy(cluster->rgeGma());
    return rgeGma_;
}

const Range& Experiment::rgeFixedInten(bool trans, bool cut) const {
    if (!rgeFixedInten_.isValid()) {
        TakesLongTime __;
        for (const Cluster* cluster : clusters_)
            for (const Measurement* one : cluster->members()) {
                if (one->image()) {
                    const shp_Image& image = one->image();
                    shp_ImageLens imageLens = gSession->imageLens(*image, trans, cut);
                    rgeFixedInten_.extendBy(imageLens->rgeInten(false));
                }
            }
    }
    return rgeFixedInten_;
}

Curve Experiment::avgCurve() const {
    if (avgCurve_.isEmpty()) {
        TakesLongTime __;
        qDebug() << "averaging curve over experiment sized " << size();
        computeAvgeCurve();
    }
    return avgCurve_;
}

void Experiment::invalidateAvgMutables() const {
    avgMonitorCount_ = avgDeltaMonitorCount_ = avgDeltaTime_ = NAN;
    rgeFixedInten_.invalidate();
    rgeGma_.invalidate();
    avgCurve_.clear();
}

//! Computed cached avgeCurve_.
void Experiment::computeAvgeCurve() const {
    vec<const Measurement*> group;
    for (Cluster const* cluster : clusters_)
        for (const Measurement* one: cluster->members())
            group.append(one);
    Sequence allData(group);
    avgCurve_ = gSession->defaultClusterLens(allData).makeCurve();
}

qreal Experiment::calcAvgMutable(qreal (Cluster::*avgFct)() const) const {
    qreal sum = 0;
    int cnt = 0;
    for (Cluster const* cluster : clusters_) {
        sum += ((*cluster).*avgFct)() * cluster->count();
        cnt += cluster->count();
    }
    return sum/cnt;
}

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

#include "measurement.h"
#include "core/session.h"

Experiment::Experiment(const int combineBy) : combineBy_(combineBy) {
    invalidateAvgMutables();
}

void Experiment::appendHere(shp_Cluster cluster) {
    // can be added only once
    append(cluster);
    invalidateAvgMutables();
}

size2d Experiment::imageSize() const {
    if (isEmpty())
        return size2d(0, 0);
    // all images have the same size; simply take the first one
    return first()->imageSize();
}

qreal Experiment::avgMonitorCount() const {
    if (qIsNaN(avgMonitorCount_))
        avgMonitorCount_ = calcAvgMutable(&Cluster::avgMonitorCount);
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
        for (const shp_Cluster& cluster : *this)
            rgeGma_.extendBy(cluster->rgeGma());
    return rgeGma_;
}

const Range& Experiment::rgeFixedInten(bool trans, bool cut) const {
    if (!rgeFixedInten_.isValid()) {
        TakesLongTime __;
        for (const shp_Cluster& cluster : *this)
            for (const shp_Measurement& one : *cluster) {
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
        // TODO invalidate when combinedDgram is unchecked
        TakesLongTime __;
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
    vec<shp_Measurement> group;
    for (shp_Cluster const& cluster : *this)
        for (shp_Measurement const& one : *cluster)
            group.append(one);
    Cluster allData(group);
    avgCurve_ = gSession->defaultClusterLens(allData)->makeCurve();
}

qreal Experiment::calcAvgMutable(qreal (Cluster::*avgFct)() const) const {
    qreal ret = 0;
    for (shp_Cluster const& cluster : *this)
        ret += ((*cluster).*avgFct)();
    ret /= count();
    return ret;
}

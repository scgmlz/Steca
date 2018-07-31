//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/active_clusters.h
//! @brief     Defines class ActiveClusters
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef ACTIVE_CLUSTERS_H
#define ACTIVE_CLUSTERS_H

#include "core/typ/cached.h"
#include "core/data/dfgram.h"
#include "core/raw/measurement.h"

class Cluster;
class ActiveClusters;

Curve computeAvgCurve(const ActiveClusters*const);

//! The list of activated Cluster|s, and cached averages

class ActiveClusters {
public:
    ActiveClusters()
        : avgDfgram( [this]()->Dfgram{ return Dfgram(computeAvgCurve(this)); } )
        { invalidateAvgMutables(); }
    ActiveClusters(const ActiveClusters&) = delete;

    void reset(std::vector<std::unique_ptr<Cluster>>& allClusters);

    const std::vector<Cluster*>& clusters() const { return clusters_; }
    int size() const { return clusters_.size(); }

    Cached<double> grandAvgMonitorCount {[this]()->double{
            return recomputeAvg([](const Measurement* one){
                    return one->monitorCount();});} };
    Cached<double> grandAvgDeltaMonitorCount {[this]()->double{
            return recomputeAvg([](const Measurement* one){
                    return one->deltaMonitorCount();});} };
    Cached<double> grandAvgTime {[this]()->double{
            return recomputeAvg([](const Measurement* one){
                    return one->time();});} };
    Cached<double> grandAvgDeltaTime {[this]()->double{
            return recomputeAvg([](const Measurement* one){
                    return one->deltaTime();});} };

    const Range& rgeGma() const;
    const Range& rgeFixedInten(bool trans, bool cut) const;

    void invalidateAvgMutables() const;

    Cached<Dfgram> avgDfgram;

private:
    std::vector<Cluster*> clusters_;
    double recomputeAvg(std::function<double(const Measurement*)>);
    // computed on demand (NaNs or emptiness indicate yet unknown values) // TODO use cached.h
    mutable Range rgeFixedInten_;
    mutable Range rgeGma_;
};

#endif // ACTIVE_CLUSTERS_H

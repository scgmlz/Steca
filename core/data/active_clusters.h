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
#include "core/typ/curve.h"

class Cluster;
class ActiveClusters;

Curve computeAvgCurve(const ActiveClusters*const);

//! The list of activated Cluster|s, and cached averages

class ActiveClusters {
public:
    ActiveClusters()
        : avgCurve( [this]()->Curve{ return computeAvgCurve(this); } )
        { invalidateAvgMutables(); }
    ActiveClusters(const ActiveClusters&) = delete;

    void reset(std::vector<std::unique_ptr<Cluster>>& allClusters);

    const std::vector<Cluster*>& clusters() const { return clusters_; }
    int size() const { return clusters_.size(); }

    double grandAvgMonitorCount() const;
    double grandAvgDeltaMonitorCount() const;
    double grandAvgTime() const;
    double grandAvgDeltaTime() const;

    const Range& rgeGma() const;
    const Range& rgeFixedInten(bool trans, bool cut) const;

    void invalidateAvgMutables() const;

    Cached<Curve> avgCurve;

private:
    std::vector<Cluster*> clusters_;

    // computed on demand (NaNs or emptiness indicate yet unknown values) // TODO use cached.h
    mutable double avgMonitorCount_;
    mutable double avgDeltaMonitorCount_;
    mutable double avgTime_;
    mutable double avgDeltaTime_;
    mutable Range rgeFixedInten_;
    mutable Range rgeGma_;
    mutable Curve avgCurve_;
};

#endif // ACTIVE_CLUSTERS_H

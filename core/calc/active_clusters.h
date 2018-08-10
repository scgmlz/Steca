//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/active_clusters.h
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

#include "core/data/cluster.h"
#include "core/typ/cached.h"

//! The list of activated Cluster|s, their average Dfgram, and a few more cached properties.

class ActiveClusters {
public:
    ActiveClusters();
    ActiveClusters(const ActiveClusters&) = delete;

    int size() const { return clusters.get().size(); }
    void invalidate() const;
    void invalidateAvg() const;

    Cached<std::vector<const Cluster*>> clusters;
    Cached<Dfgram> avgDfgram;
    Cached<Range> rgeGma;
    Cached<Range> rgeFixedInten;
    Cached<double> grandAvgMonitorCount;
    Cached<double> grandAvgDeltaMonitorCount;
    Cached<double> grandAvgTime;
    Cached<double> grandAvgDeltaTime;
};

#endif // ACTIVE_CLUSTERS_H

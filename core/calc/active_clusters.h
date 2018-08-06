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

#include "core/typ/cached.h"

class Cluster;
class Dfgram;
class Measurement;
class Range;

//! The list of activated Cluster|s, and cached averages

class ActiveClusters {
public:
    ActiveClusters();
    ActiveClusters(const ActiveClusters&) = delete;

    void invalidate() const;
    void invalidateAvg() const;

    Cached<std::vector<Cluster*>> clusters;
    Cached<Dfgram> avgDfgram;
    Cached<Range> rgeGma;
    Cached<Range> rgeFixedInten;
    Cached<double> grandAvgMonitorCount;
    Cached<double> grandAvgDeltaMonitorCount;
    Cached<double> grandAvgTime;
    Cached<double> grandAvgDeltaTime;

private:
    double recomputeAvg(std::function<double(const Measurement*)> f);
};

#endif // ACTIVE_CLUSTERS_H

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

#include "core/typ/curve.h"

struct size2d;
class Cluster;
class PeakInfos;

//! The list of activated Cluster|s, and cached averages

class ActiveClusters {
public:
    ActiveClusters();

    void appendHere(Cluster*);

    const std::vector<Cluster*>& clusters() const { return clusters_; }
    int size() const { return clusters_.size(); }
    size2d imageSize() const;
    double avgMonitorCount() const;
    double avgDeltaMonitorCount() const;
    double avgTime() const;
    double avgDeltaTime() const;
    const Range& rgeGma() const;
    const Range& rgeFixedInten(bool trans, bool cut) const;
    Curve avgCurve() const;

    void invalidateAvgMutables() const;

private:
    void computeAvgCurve() const;
    double calcAvgMutable(double (Cluster::*avgFct)() const) const;

    std::vector<Cluster*> clusters_;

    // computed on demand (NaNs or emptiness indicate yet unknown values)
    mutable double avgMonitorCount_;
    mutable double avgDeltaMonitorCount_;
    mutable double avgTime_;
    mutable double avgDeltaTime_;
    mutable Range rgeFixedInten_;
    mutable Range rgeGma_;
    mutable Curve avgCurve_;
};

#endif // ACTIVE_CLUSTERS_H

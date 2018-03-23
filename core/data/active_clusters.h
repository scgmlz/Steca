// ************************************************************************** //
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
// ************************************************************************** //

#ifndef ACTIVE_CLUSTERS_H
#define ACTIVE_CLUSTERS_H

#include "core/data/cluster.h" // no auto rm
#include <QSharedPointer> // required by some compilers

//! The list of activated Cluster|s, and cached averages

class ActiveClusters {
public:
    ActiveClusters();

    void appendHere(const Cluster*);

    const std::vector<const Cluster*>& clusters() const { return clusters_; }
    int size() const { return clusters_.size(); }
    size2d imageSize() const;
    qreal avgMonitorCount() const;
    qreal avgDeltaMonitorCount() const;
    qreal avgDeltaTime() const;
    const Range& rgeGma() const;
    const Range& rgeFixedInten(bool trans, bool cut) const;
    Curve avgCurve() const;

    void invalidateAvgMutables() const;
    PeakInfos rawFits(const class Peak& peak, class Progress* progress) const;


private:
    void computeAvgeCurve() const;
    qreal calcAvgMutable(qreal (Cluster::*avgFct)() const) const;

    std::vector<const Cluster*> clusters_;

    // computed on demand (NaNs or emptiness indicate yet unknown values)
    mutable qreal avgMonitorCount_;
    mutable qreal avgDeltaMonitorCount_;
    mutable qreal avgDeltaTime_;
    mutable Range rgeFixedInten_;
    mutable Range rgeGma_;
    mutable Curve avgCurve_;
};

#endif // ACTIVE_CLUSTERS_H

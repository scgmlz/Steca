// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/experiment.h
//! @brief     Defines class Experiment
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "core/data/cluster.h" // no auto rm
#include "core/typ/curve.h"
#include <QSharedPointer> // required by some compilers

//! A sequence of Cluster|s

class Experiment final : public vec<shp_Cluster> {
public:
    Experiment();

    void appendHere(shp_Cluster);

    size2d imageSize() const;

    qreal avgMonitorCount() const;
    qreal avgDeltaMonitorCount() const;
    qreal avgDeltaTime() const;

    const Range& rgeGma() const;
    const Range& rgeFixedInten(bool trans, bool cut) const;

    Curve avgCurve() const;

    void invalidateAvgMutables() const;

private:
    void computeAvgeCurve() const;
    qreal calcAvgMutable(qreal (Cluster::*avgFct)() const) const;

    // computed on demand (NaNs or emptiness indicate yet unknown values)
    mutable qreal avgMonitorCount_;
    mutable qreal avgDeltaMonitorCount_;
    mutable qreal avgDeltaTime_;
    mutable Range rgeFixedInten_;
    mutable Range rgeGma_;
    mutable Curve avgCurve_;
};

#endif // EXPERIMENT_H

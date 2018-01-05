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

#include "suite.h" // no auto rm
#include <QSharedPointer> // required by some compilers

//! A sequence of Suites's

class Experiment final : public vec<QSharedPointer<Suite>> {
public:
    Experiment();

    void appendHere(QSharedPointer<Suite>);

    size2d imageSize() const;

    qreal avgMonitorCount() const;
    qreal avgDeltaMonitorCount() const;
    qreal avgDeltaTime() const;

    Range const& rgeGma() const;
    Range const& rgeFixedInten(bool trans, bool cut) const;

    Curve avgCurve() const;

    void invalidateAvgMutables() const;

private:
    QSharedPointer<Suite> combineAll() const;
    qreal calcAvgMutable(qreal (Suite::*avgMth)() const) const;

    // computed on demand (NaNs or emptiness indicate yet unknown values)
    mutable qreal avgMonitorCount_, avgDeltaMonitorCount_, avgDeltaTime_;
    mutable Range rgeFixedInten_;
    mutable Range rgeGma_;
    mutable Curve avgCurve_;
};

#endif // EXPERIMENT_H

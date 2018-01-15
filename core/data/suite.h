// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/suite.h
//! @brief     Defines class Suite
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SUITE_H
#define SUITE_H

#include "core/typ/angles.h"
#include "core/data/image.h"
#include "core/data/measurement.h"
#include <QSharedPointer> // no auto rm

class Experiment;

//! One or more Measurement's

class Suite final : public vec<shp_Measurement> {
public:
    Suite();

    shp_Metadata metadata() const;
    Experiment const& experiment() const;

    deg omg() const;
    deg phi() const;
    deg chi() const;

    Range rgeGma() const;
    Range rgeGmaFull() const;
    Range rgeTth() const;

    Range rgeInten() const;

    qreal avgMonitorCount() const;
    qreal avgDeltaMonitorCount() const;
    qreal avgDeltaTime() const;

    inten_vec collectIntens(const Image* intensCorr, const Range&) const;
    void calculateAlphaBeta(deg tth, deg gma, deg& alpha, deg& beta) const;

    size2d imageSize() const;

private:
    Experiment* experiment_;
    shp_Metadata md_; // on demand, compute once

    void compute_metadata() const;

    friend class Experiment;
};

typedef QSharedPointer<Suite> shp_Suite;

Q_DECLARE_METATYPE(shp_Suite)

#endif // SUITE_H

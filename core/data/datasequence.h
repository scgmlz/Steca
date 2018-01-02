// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/datasequence.h
//! @brief     Defines class DataSequence
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DATASEQUENCE_H
#define DATASEQUENCE_H

#include "typ/angles.h"
#include "typ/curve.h"
#include "data/image.h"
#include "typ/variant.h"

class Session;
class Metadata;
class Measurement;
class Experiment;

//! One or more Measurement's

class DataSequence final : public vec<QSharedPointer<Measurement const>> {
private:
    friend class Experiment;

public:
    DataSequence();

    QSharedPointer<Metadata const> metadata() const;
    Experiment const& datasequence() const;

    deg omg() const;
    deg phi() const;
    deg chi() const;

    Range rgeGma(Session const&) const;
    Range rgeGmaFull(Session const&) const;
    Range rgeTth(Session const&) const;

    Range rgeInten() const;

    qreal avgMonitorCount() const;
    qreal avgDeltaMonitorCount() const;
    qreal avgDeltaTime() const;

    inten_vec collectIntens(Session const&, Image const* intensCorr, Range const&) const;
    void calculateAlphaBeta(deg tth, deg gma, deg& alpha, deg& beta) const;

    size2d imageSize() const;

private:
    Experiment* experiment_; // here it belongs (or can be nullptr)
    QSharedPointer<Metadata const> md_; // on demand, compute once
};

Q_DECLARE_METATYPE(QSharedPointer<DataSequence>)

#endif // DATASEQUENCE_H

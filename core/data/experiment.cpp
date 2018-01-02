// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/experiment.cpp
//! @brief     Implements class Experiment
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "experiment.h"
#include "datasequence.h"
#include "def/idiomatic_for.h"
#include "measurement.h"
#include "metadata.h"
#include "session.h"
#include "typ/array2d.h"
#include "typ/curve.h"
#include "typ/matrix.h"
#include <qmath.h>

Experiment const& DataSequence::datasequence() const {
    debug::ensure(datasequence_);
    return *datasequence_;
}

Experiment::Experiment() {
    invalidateAvgMutables();
}

void Experiment::appendHere(QSharedPointer<DataSequence> dataset) {
    // can be added only once
    debug::ensure(!dataset->datasequence_);
    dataset->datasequence_ = this;
    append(dataset);
    invalidateAvgMutables();
}

size2d Experiment::imageSize() const {
    if (isEmpty())
        return size2d(0, 0);
    // all images have the same size; simply take the first one
    return first()->imageSize();
}

qreal Experiment::avgMonitorCount() const {
    if (qIsNaN(avgMonitorCount_))
        avgMonitorCount_ = calcAvgMutable(&DataSequence::avgMonitorCount);
    return avgMonitorCount_;
}

qreal Experiment::avgDeltaMonitorCount() const {
    if (qIsNaN(avgDeltaMonitorCount_))
        avgDeltaMonitorCount_ = calcAvgMutable(&DataSequence::avgDeltaMonitorCount);
    return avgDeltaMonitorCount_;
}

qreal Experiment::avgDeltaTime() const {
    if (qIsNaN(avgDeltaTime_))
        avgDeltaTime_ = calcAvgMutable(&DataSequence::avgDeltaTime);
    return avgDeltaTime_;
}

Range const& Experiment::rgeGma(Session const& session) const {
    if (!rgeGma_.isValid())
        for (auto& dataset : *this)
            rgeGma_.extendBy(dataset->rgeGma(session));
    return rgeGma_;
}

Range const& Experiment::rgeFixedInten(Session const& session, bool trans, bool cut) const {
    if (!rgeFixedInten_.isValid()) {
        TakesLongTime __;
        for (auto& dataset : *this)
            for (auto& one : *dataset) {
                if (one->image()) {
                    auto& image = *one->image();
                    calc::shp_ImageLens imageLens = session.imageLens(image, *this, trans, cut);
                    rgeFixedInten_.extendBy(imageLens->rgeInten(false));
                }
            }
    }
    return rgeFixedInten_;
}

Curve Experiment::avgCurve(Session const& session) const {
    if (avgCurve_.isEmpty()) {
        // TODO invalidate when combinedDgram is unchecked
        TakesLongTime __;
        avgCurve_ =
            session.datasetLens(*combineAll(), *this, session.norm(), true, true)->makeCurve();
    }
    return avgCurve_;
}

void Experiment::invalidateAvgMutables() const {
    avgMonitorCount_ = avgDeltaMonitorCount_ = avgDeltaTime_ = NAN;
    rgeFixedInten_.invalidate();
    rgeGma_.invalidate();
    avgCurve_.clear();
}

QSharedPointer<DataSequence> Experiment::combineAll() const {
    QSharedPointer<DataSequence> d(new DataSequence);
    for (QSharedPointer<DataSequence> const& dataset : *this)
        for (QSharedPointer<Measurement const> const& one : *dataset)
            d->append(one);
    return d;
}

qreal Experiment::calcAvgMutable(qreal (DataSequence::*avgMth)() const) const {
    qreal avg = 0;
    if (!isEmpty()) {
        for (auto& dataset : *this)
            avg += ((*dataset).*avgMth)();
        avg /= count();
    }
    return avg;
}

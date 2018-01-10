// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/experiment.cpp
//! @brief     Implements class Experiment
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "measurement.h"
#include "core/session.h"

Experiment::Experiment() {
    invalidateAvgMutables();
}

void Experiment::appendHere(shp_Suite dataseq) {
    // can be added only once
    debug::ensure(!dataseq->experiment_);
    dataseq->experiment_ = this;
    append(dataseq);
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
        avgMonitorCount_ = calcAvgMutable(&Suite::avgMonitorCount);
    return avgMonitorCount_;
}

qreal Experiment::avgDeltaMonitorCount() const {
    if (qIsNaN(avgDeltaMonitorCount_))
        avgDeltaMonitorCount_ = calcAvgMutable(&Suite::avgDeltaMonitorCount);
    return avgDeltaMonitorCount_;
}

qreal Experiment::avgDeltaTime() const {
    if (qIsNaN(avgDeltaTime_))
        avgDeltaTime_ = calcAvgMutable(&Suite::avgDeltaTime);
    return avgDeltaTime_;
}

const Range& Experiment::rgeGma() const {
    if (!rgeGma_.isValid())
        for (auto& dataseq : *this)
            rgeGma_.extendBy(dataseq->rgeGma());
    return rgeGma_;
}

const Range& Experiment::rgeFixedInten(bool trans, bool cut) const {
    if (!rgeFixedInten_.isValid()) {
        TakesLongTime __;
        for (auto& dataseq : *this)
            for (auto& one : *dataseq) {
                if (one->image()) {
                    auto& image = *one->image();
                    shp_ImageLens imageLens = gSession->imageLens(image, trans, cut);
                    rgeFixedInten_.extendBy(imageLens->rgeInten(false));
                }
            }
    }
    return rgeFixedInten_;
}

Curve Experiment::avgCurve() const {
    if (avgCurve_.isEmpty()) {
        // TODO invalidate when combinedDgram is unchecked
        TakesLongTime __;
        avgCurve_ = gSession->dataseqLens(*combineAll(), gSession->norm(), true, true)->makeCurve();
    }
    return avgCurve_;
}

void Experiment::invalidateAvgMutables() const {
    avgMonitorCount_ = avgDeltaMonitorCount_ = avgDeltaTime_ = NAN;
    rgeFixedInten_.invalidate();
    rgeGma_.invalidate();
    avgCurve_.clear();
}

shp_Suite Experiment::combineAll() const {
    shp_Suite ret(new Suite);
    for (shp_Suite const& dataseq : *this)
        for (shp_Measurement const& one : *dataseq)
            ret->append(one);
    return ret;
}

qreal Experiment::calcAvgMutable(qreal (Suite::*avgMth)() const) const {
    qreal ret = 0;
    if (!isEmpty()) {
        for (auto& dataseq : *this)
            ret += ((*dataseq).*avgMth)();
        ret /= count();
    }
    return ret;
}

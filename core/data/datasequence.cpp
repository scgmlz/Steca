// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/datasequence.cpp
//! @brief     Implements classes Measurement, DataSequence, Experiment
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "datasequence.h"
#include "measurement.h"
#include "metadata.h"
#include "def/idiomatic_for.h"
#include "session.h"
#include "typ/array2d.h"
#include "typ/curve.h"
#include "typ/matrix.h"
#include <qmath.h>

// ************************************************************************** //
//  class OneDataset
// ************************************************************************** //

Measurement::Measurement(Metadata const& md, size2d const& size, inten_vec const& intens)
    : md_(new Metadata(md)), image_(new Image(size)) {
    debug::ensure(intens.count() == size.count());
    for_i (intens.count())
        image_->setInten(i, intens.at(i));
}

QSharedPointer<Metadata const> Measurement::metadata() const {
    debug::ensure(!md_.isNull());
    return md_;
}

Range Measurement::rgeGma(Session const& session) const {
    return session.angleMap(*this)->rgeGma();
}

Range Measurement::rgeGmaFull(Session const& session) const {
    return session.angleMap(*this)->rgeGmaFull();
}

Range Measurement::rgeTth(Session const& session) const {
    return session.angleMap(*this)->rgeTth();
}

Range Measurement::rgeInten() const {
    return image_->rgeInten();
}

size2d Measurement::imageSize() const {
    return image_->size();
}

void Measurement::collectIntens(
    Session const& session, Image const* intensCorr, inten_vec& intens, uint_vec& counts,
    Range const& rgeGma, deg minTth, deg deltaTth) const {
    auto angleMap = session.angleMap(*this);
    debug::ensure(!angleMap.isNull());
    AngleMap const& map = *angleMap;

    uint_vec const* gmaIndexes = nullptr;
    uint gmaIndexMin = 0, gmaIndexMax = 0;
    map.getGmaIndexes(rgeGma, gmaIndexes, gmaIndexMin, gmaIndexMax);

    debug::ensure(gmaIndexes);
    debug::ensure(gmaIndexMin <= gmaIndexMax);
    debug::ensure(gmaIndexMax <= gmaIndexes->count());

    debug::ensure(intens.count() == counts.count());
    uint count = intens.count();

    debug::ensure(0 < deltaTth);

    for (uint i = gmaIndexMin; i < gmaIndexMax; ++i) {
        uint ind = gmaIndexes->at(i);
        inten_t inten = image_->inten(ind);
        if (qIsNaN(inten))
            continue;

        inten_t corr = intensCorr ? intensCorr->inten(ind) : 1;
        if (qIsNaN(corr))
            continue;

        inten *= corr;

        deg tth = map.at(ind).tth;

        // bin index
        uint ti = to_u(qFloor((tth - minTth) / deltaTth));
        debug::ensure(ti <= count);
        ti = qMin(ti, count - 1); // it can overshoot due to floating point calculation

        intens[ti] += inten;
        counts[ti] += 1;
    }
}

deg Measurement::midTth() const { return md_->motorTth; }

qreal Measurement::monitorCount() const { return md_->monitorCount; }
qreal Measurement::deltaMonitorCount() const { return md_->deltaMonitorCount; }
qreal Measurement::deltaTime() const { return md_->deltaTime; }

deg Measurement::omg() const { return md_->motorOmg; }
deg Measurement::phi() const { return md_->motorPhi; }
deg Measurement::chi() const { return md_->motorChi; }


// ************************************************************************** //
//  class Dataset
// ************************************************************************** //

DataSequence::DataSequence() : experiment_(nullptr) {}

QSharedPointer<Metadata const> DataSequence::metadata() const {
    if (md_.isNull()) {
        debug::ensure(!isEmpty());
        const_cast<DataSequence*>(this)->md_ = QSharedPointer<Metadata const>(new Metadata);
        Metadata* m = const_cast<Metadata*>(md_.data());

        debug::ensure(!first()->metadata().isNull());
        Metadata const& firstMd = *(first()->metadata());

        m->date = firstMd.date;
        m->comment = firstMd.comment;

        // sums: delta mon. count and time,
        // takes the last ones (presumed the maximum) of mon. count and time,
        // averages the rest
        for (auto& one : *this) {
            Metadata const* d = one->metadata().data();
            debug::ensure(d);

            m->motorXT += d->motorXT;
            m->motorYT += d->motorYT;
            m->motorZT += d->motorZT;

            m->motorOmg += d->motorOmg;
            m->motorTth += d->motorTth;
            m->motorPhi += d->motorPhi;
            m->motorChi += d->motorChi;

            m->motorPST += d->motorPST;
            m->motorSST += d->motorSST;
            m->motorOMGM += d->motorOMGM;

            m->nmT += d->nmT;
            m->nmTeload += d->nmTeload;
            m->nmTepos += d->nmTepos;
            m->nmTeext += d->nmTeext;
            m->nmXe += d->nmXe;
            m->nmYe += d->nmYe;
            m->nmZe += d->nmZe;

            m->deltaMonitorCount += d->deltaMonitorCount;
            m->deltaTime += d->deltaTime;

            if (m->monitorCount > d->monitorCount)
                qWarning() << "decreasing monitor count in combined datasequence";
            if (m->time > d->time)
                qWarning() << "decreasing time in combined datasequence";
            m->monitorCount = d->monitorCount;
            m->time = d->time;
        }

        qreal fac = 1.0 / count();

        m->motorXT *= fac;
        m->motorYT *= fac;
        m->motorZT *= fac;

        m->motorOmg *= fac;
        m->motorTth *= fac;
        m->motorPhi *= fac;
        m->motorChi *= fac;

        m->motorPST *= fac;
        m->motorSST *= fac;
        m->motorOMGM *= fac;

        m->nmT *= fac;
        m->nmTeload *= fac;
        m->nmTepos *= fac;
        m->nmTeext *= fac;
        m->nmXe *= fac;
        m->nmYe *= fac;
        m->nmZe *= fac;
    }

    return md_;
}

#define AVG_ONES(what)                                                                             \
    debug::ensure(!isEmpty());                                                                 \
    qreal avg = 0;                                                                                 \
    for (auto& one : *this)                                                                        \
        avg += one->what();                                                                        \
    avg /= count();                                                                                \
    return avg;

deg DataSequence::omg() const { AVG_ONES(omg) }

deg DataSequence::phi() const { AVG_ONES(phi) }

deg DataSequence::chi() const { AVG_ONES(chi) }

// combined range of combined datasequence
#define RGE_COMBINE(combineOp, what)                                                               \
    debug::ensure(!isEmpty());                                                                           \
    Range rge;                                                                                     \
    for (auto& one : *this)                                                                        \
        rge.combineOp(one->what);                                                                  \
    return rge;

Range DataSequence::rgeGma(Session const& session) const { RGE_COMBINE(extendBy, rgeGma(session)) }

Range DataSequence::rgeGmaFull(Session const& session) const {
    RGE_COMBINE(extendBy, rgeGmaFull(session))
}

Range DataSequence::rgeTth(Session const& session) const { RGE_COMBINE(extendBy, rgeTth(session)) }

Range DataSequence::rgeInten() const { RGE_COMBINE(intersect, rgeInten()) }

qreal DataSequence::avgMonitorCount() const { AVG_ONES(monitorCount) }

qreal DataSequence::avgDeltaMonitorCount() const { AVG_ONES(deltaMonitorCount) }

qreal DataSequence::avgDeltaTime() const { AVG_ONES(deltaTime) }

inten_vec DataSequence::collectIntens(
    Session const& session, Image const* intensCorr, Range const& rgeGma) const {
    Range tthRge = rgeTth(session);
    deg tthWdt = tthRge.width();

    auto cut = session.imageCut();
    uint pixWidth = session.imageSize().w - cut.left - cut.right;

    uint numBins;
    if (1 < count()) { // combined datasequence
        auto one = first();
        deg delta = one->rgeTth(session).width() / pixWidth;
        numBins = to_u(qCeil(tthWdt / delta));
    } else {
        numBins = pixWidth; // simply match the pixels
    }

    inten_vec intens(numBins, 0);
    uint_vec counts(numBins, 0);

    deg minTth = tthRge.min, deltaTth = tthWdt / numBins;

    for (auto& one : *this)
        one->collectIntens(session, intensCorr, intens, counts, rgeGma, minTth, deltaTth);

    // sum or average
    if (session.intenScaledAvg()) {
        preal scale = session.intenScale();
        for_i (numBins) {
            auto cnt = counts.at(i);
            if (cnt > 0)
                intens[i] *= scale / cnt;
        }
    }

    return intens;
}

size2d DataSequence::imageSize() const {
    debug::ensure(!isEmpty());
    // all images have the same size; simply take the first one
    return first()->imageSize();
}

//! Calculates the polefigure coordinates alpha and beta with regards to
//! sample orientation and diffraction angles.

//! tth: Center of reflection's 2theta interval.
//! gma: Center of gamma slice.
void DataSequence::calculateAlphaBeta(deg tth, deg gma, deg& alpha, deg& beta) const {

    // Rotate a unit vector initially parallel to the y axis with regards to the
    // angles. As a result, the vector is a point on a unit sphere
    // corresponding to the location of a polefigure point.
    // Note that the rotations here do not correspond to C. Randau's dissertation.
    // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
    vec3r rotated = mat3r::rotationCWz(phi()) * mat3r::rotationCWx(chi())
        * mat3r::rotationCWz(omg()) * mat3r::rotationCWx(gma.toRad())
        * mat3r::rotationCCWz(tth.toRad() / 2) * vec3r(0, 1, 0);

    // Extract alpha (latitude) and beta (longitude).
    rad alphaRad = acos(rotated._2);
    rad betaRad = atan2(rotated._0, rotated._1);

    // If alpha is in the wrong hemisphere, mirror both alpha and beta over the
    // center of a unit sphere.
    if (alphaRad > M_PI_2) { // REVIEW - seems like that happens only for a very narrow ring
        alphaRad = qAbs(alphaRad - M_PI);
        betaRad = betaRad + (betaRad < 0 ? M_PI : -M_PI);
    }

    // Keep beta between 0 and 2pi.
    if (betaRad < 0)
        betaRad = betaRad + 2 * M_PI;

    alpha = alphaRad.toDeg();
    beta = betaRad.toDeg();
}

// ************************************************************************** //
//  class Datasets
// ************************************************************************** //

Experiment const& DataSequence::datasequence() const {
    debug::ensure(experiment_);
    return *experiment_;
}

Experiment::Experiment() {
    invalidateAvgMutables();
}

void Experiment::appendHere(QSharedPointer<DataSequence> dataset) {
    // can be added only once
    debug::ensure(!dataset->experiment_);
    dataset->experiment_ = this;
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

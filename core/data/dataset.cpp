// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/dataset.cpp
//! @brief     Implements classes [One]Dataset[s]
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "dataset.h"
#include "data/metadata.h"
#include "def/idiomatic_for.h"
#include "session.h"
#include "typ/array2d.h"
#include "typ/curve.h"
#include "typ/image.h"
#include "typ/matrix.h"
#include <qmath.h>

using typ::Curve;
using typ::Image;
using typ::Range;
using typ::size2d;

// ************************************************************************** //
//  class OneDataset
// ************************************************************************** //

OneDataset::OneDataset(Metadata const& md, typ::inten_arr const& intens)
    : md_(new Metadata(md)), image_(new Image(intens)) {}

OneDataset::OneDataset(Metadata const& md, size2d const& size, inten_vec const& intens)
    : md_(new Metadata(md)), image_(new Image(size)) {
    debug::ensure(intens.count() == size.count());
    for_i (intens.count())
        image_->setInten(i, intens.at(i));
}

OneDataset::OneDataset(OneDataset const& that) : md_(that.md_), image_(that.image_) {}

QSharedPointer<Metadata const> OneDataset::metadata() const {
    debug::ensure(!md_.isNull());
    return md_;
}

typ::Range OneDataset::rgeGma(Session const& session) const {
    return session.angleMap(*this)->rgeGma();
}

typ::Range OneDataset::rgeGmaFull(Session const& session) const {
    return session.angleMap(*this)->rgeGmaFull();
}

typ::Range OneDataset::rgeTth(Session const& session) const {
    return session.angleMap(*this)->rgeTth();
}

typ::Range OneDataset::rgeInten() const {
    return image_->rgeInten();
}

size2d OneDataset::imageSize() const {
    return image_->size();
}

void OneDataset::collectIntens(
    Session const& session, typ::Image const* intensCorr, inten_vec& intens, uint_vec& counts,
    typ::Range const& rgeGma, deg minTth, deg deltaTth) const {
    auto angleMap = session.angleMap(*this);
    debug::ensure(!angleMap.isNull());
    typ::AngleMap const& map = *angleMap;

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

typ::deg OneDataset::midTth() const { return md_->motorTth; }

qreal OneDataset::monitorCount() const { return md_->monitorCount; }
qreal OneDataset::deltaMonitorCount() const { return md_->deltaMonitorCount; }
qreal OneDataset::deltaTime() const { return md_->deltaTime; }

typ::deg OneDataset::omg() const { return md_->motorOmg; }
typ::deg OneDataset::phi() const { return md_->motorPhi; }
typ::deg OneDataset::chi() const { return md_->motorChi; }


// ************************************************************************** //
//  class Dataset
// ************************************************************************** //

Dataset::Dataset() : datasets_(nullptr) {}

QSharedPointer<Metadata const> Dataset::metadata() const {
    if (md_.isNull()) {
        debug::ensure(!isEmpty());
        const_cast<Dataset*>(this)->md_ = QSharedPointer<Metadata const>(new Metadata);
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
                qWarning() << "decreasing monitor count in combined datasets";
            if (m->time > d->time)
                qWarning() << "decreasing time in combined datasets";
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

deg Dataset::omg() const { AVG_ONES(omg) }

deg Dataset::phi() const { AVG_ONES(phi) }

deg Dataset::chi() const { AVG_ONES(chi) }

// combined range of combined datasets
#define RGE_COMBINE(combineOp, what)                                                               \
    debug::ensure(!isEmpty());                                                                           \
    Range rge;                                                                                     \
    for (auto& one : *this)                                                                        \
        rge.combineOp(one->what);                                                                  \
    return rge;

typ::Range Dataset::rgeGma(Session const& session) const { RGE_COMBINE(extendBy, rgeGma(session)) }

typ::Range Dataset::rgeGmaFull(Session const& session) const {
    RGE_COMBINE(extendBy, rgeGmaFull(session))
}

typ::Range Dataset::rgeTth(Session const& session) const { RGE_COMBINE(extendBy, rgeTth(session)) }

typ::Range Dataset::rgeInten() const { RGE_COMBINE(intersect, rgeInten()) }

qreal Dataset::avgMonitorCount() const { AVG_ONES(monitorCount) }

qreal Dataset::avgDeltaMonitorCount() const { AVG_ONES(deltaMonitorCount) }

qreal Dataset::avgDeltaTime() const { AVG_ONES(deltaTime) }

inten_vec Dataset::collectIntens(
    Session const& session, typ::Image const* intensCorr, typ::Range const& rgeGma) const {
    typ::Range tthRge = rgeTth(session);
    deg tthWdt = tthRge.width();

    auto cut = session.imageCut();
    uint pixWidth = session.imageSize().w - cut.left - cut.right;

    uint numBins;
    if (1 < count()) { // combined datasets
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

size2d Dataset::imageSize() const {
    debug::ensure(!isEmpty());
    // all images have the same size; simply take the first one
    return first()->imageSize();
}

//! Calculates the polefigure coordinates alpha and beta with regards to
//! sample orientation and diffraction angles.

//! tth: Center of reflection's 2theta interval.
//! gma: Center of gamma slice.
void Dataset::calculateAlphaBeta(deg tth, deg gma, deg& alpha, deg& beta) const {

    // Rotate a unit vector initially parallel to the y axis with regards to the
    // angles. As a result, the vector is a point on a unit sphere
    // corresponding to the location of a polefigure point.
    // Note that the rotations here do not correspond to C. Randau's dissertation.
    // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
    typ::vec3r rotated = typ::mat3r::rotationCWz(phi()) * typ::mat3r::rotationCWx(chi())
        * typ::mat3r::rotationCWz(omg()) * typ::mat3r::rotationCWx(gma.toRad())
        * typ::mat3r::rotationCCWz(tth.toRad() / 2) * typ::vec3r(0, 1, 0);

    // Extract alpha (latitude) and beta (longitude).
    typ::rad alphaRad = acos(rotated._2);
    typ::rad betaRad = atan2(rotated._0, rotated._1);

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

Datasets const& Dataset::datasets() const {
    debug::ensure(datasets_);
    return *datasets_;
}

Datasets::Datasets() {
    invalidateAvgMutables();
}

void Datasets::appendHere(shp_Dataset dataset) {
    // can be added only once
    debug::ensure(!dataset->datasets_);
    dataset->datasets_ = this;
    super::append(dataset);
    invalidateAvgMutables();
}

size2d Datasets::imageSize() const {
    if (isEmpty())
        return size2d(0, 0);
    // all images have the same size; simply take the first one
    return first()->imageSize();
}

qreal Datasets::avgMonitorCount() const {
    if (qIsNaN(avgMonitorCount_))
        avgMonitorCount_ = calcAvgMutable(&Dataset::avgMonitorCount);
    return avgMonitorCount_;
}

qreal Datasets::avgDeltaMonitorCount() const {
    if (qIsNaN(avgDeltaMonitorCount_))
        avgDeltaMonitorCount_ = calcAvgMutable(&Dataset::avgDeltaMonitorCount);
    return avgDeltaMonitorCount_;
}

qreal Datasets::avgDeltaTime() const {
    if (qIsNaN(avgDeltaTime_))
        avgDeltaTime_ = calcAvgMutable(&Dataset::avgDeltaTime);
    return avgDeltaTime_;
}

typ::Range const& Datasets::rgeGma(Session const& session) const {
    if (!rgeGma_.isValid())
        for (auto& dataset : *this)
            rgeGma_.extendBy(dataset->rgeGma(session));
    return rgeGma_;
}

typ::Range const& Datasets::rgeFixedInten(Session const& session, bool trans, bool cut) const {
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

Curve Datasets::avgCurve(Session const& session) const {
    if (avgCurve_.isEmpty()) {
        // TODO invalidate when combinedDgram is unchecked
        TakesLongTime __;
        avgCurve_ =
            session.datasetLens(*combineAll(), *this, session.norm(), true, true)->makeCurve();
    }
    return avgCurve_;
}

void Datasets::invalidateAvgMutables() const {
    avgMonitorCount_ = avgDeltaMonitorCount_ = avgDeltaTime_ = NAN;
    rgeFixedInten_.invalidate();
    rgeGma_.invalidate();
    avgCurve_.clear();
}

shp_Dataset Datasets::combineAll() const {
    shp_Dataset d(new Dataset);
    for (shp_Dataset const& dataset : *this)
        for (shp_OneDataset const& one : *dataset)
            d->append(one);
    return d;
}

qreal Datasets::calcAvgMutable(qreal (Dataset::*avgMth)() const) const {
    qreal avg = 0;
    if (!isEmpty()) {
        for (auto& dataset : *this)
            avg += ((*dataset).*avgMth)();
        avg /= super::count();
    }
    return avg;
}

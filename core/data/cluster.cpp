// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/cluster.cpp
//! @brief     Implements classes Sequence, Cluster
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/session.h"
#include "core/typ/matrix.h"
#include <qmath.h>

// ************************************************************************** //
//  class Sequence
// ************************************************************************** //

Sequence::Sequence(const QVector<const Measurement*>& measurements)
    : members_(measurements)
{
}

//! Returns metadata, averaged over Sequence members. Result is cached.
shp_Metadata Sequence::avgeMetadata() const {
    if (md_.isNull())
        compute_metadata();
    return md_;
}

#define AVG_ONES(what_function)                                                  \
    qreal avg = 0;                                                      \
    for (const Measurement* one : members_)                            \
        avg += one->what_function();                                             \
    avg /= count();                                                     \
    return avg;

deg Sequence::omg() const { AVG_ONES(omg) }

deg Sequence::phi() const { AVG_ONES(phi) }

deg Sequence::chi() const { AVG_ONES(chi) }

// combined range of combined cluster
#define RGE_COMBINE(combineOp, what)                                    \
    Range rge;                                                          \
    for (const Measurement* one : members_)                            \
        rge.combineOp(one->what);                                       \
    return rge;

Range Sequence::rgeGma() const { RGE_COMBINE(extendBy, rgeGma()) }

Range Sequence::rgeGmaFull() const {
    RGE_COMBINE(extendBy, rgeGmaFull())
}

Range Sequence::rgeTth() const { RGE_COMBINE(extendBy, rgeTth()) }

Range Sequence::rgeInten() const { RGE_COMBINE(intersect, rgeInten()) }

qreal Sequence::avgMonitorCount() const { AVG_ONES(monitorCount) }

qreal Sequence::avgDeltaMonitorCount() const { AVG_ONES(deltaMonitorCount) }

qreal Sequence::avgDeltaTime() const { AVG_ONES(deltaTime) }

size2d Sequence::imageSize() const {
    // all images have the same size; simply take the first one
    return first()->imageSize();
}

//! Calculates the polefigure coordinates alpha and beta with regards to
//! sample orientation and diffraction angles.

//! tth: Center of peak's 2theta interval.
//! gma: Center of gamma slice.
void Sequence::calculateAlphaBeta(deg tth, deg gma, deg& alpha, deg& beta) const {

    // Rotate a unit vector initially parallel to the y axis with regards to the
    // angles. As a result, the vector is a point on a unit sphere
    // corresponding to the location of a polefigure point.
    // Note that the rotations here do not correspond to C. Randau's dissertation.
    // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
    vec3r rotated = mat3r::rotationCWz(phi().toRad())
        * mat3r::rotationCWx(chi().toRad())
        * mat3r::rotationCWz(omg().toRad())
        * mat3r::rotationCWx(gma.toRad())
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

Curve Sequence::toCurve() const {
    return toCurve(rgeGma());
};

Curve Sequence::toCurve(const Range& _rgeGma) const {
    double _normFactor = normFactor();
    return toCurve(_normFactor, rgeGma());
};

Curve Sequence::toCurve(qreal _normFactor) const {
    return toCurve(_normFactor, rgeGma());
};

Curve Sequence::toCurve(qreal _normFactor, const Range& _rgeGma) const {
    inten_vec intens = collectIntens(gSession->intensCorr(), _rgeGma);
    int count = intens.count();
    if (!count)
        return {};
    Curve res;
    Range _rgeTth = rgeTth();
    deg minTth = _rgeTth.min;
    deg deltaTth = _rgeTth.width() / count;
    for_i (count)
        res.append(minTth + deltaTth * i, qreal(intens.at(i) * _normFactor));
    return res;
};

qreal Sequence::normFactor() const {
    return normFactor(gSession->norm());
};

qreal Sequence::normFactor(eNorm norm) const {
    qreal num = 1, den = 1;

    switch (norm) {
    case eNorm::MONITOR:
        num = gSession->experiment().avgMonitorCount();
        den = avgMonitorCount();
        break;
    case eNorm::DELTA_MONITOR:
        num = gSession->experiment().avgDeltaMonitorCount();
        den = avgDeltaMonitorCount();
        break;
    case eNorm::DELTA_TIME:
        num = gSession->experiment().avgDeltaTime();
        den = avgDeltaTime();
        break;
    case eNorm::BACKGROUND:
        num = gSession->calcAvgBackground();
        den = gSession->calcAvgBackground(*this);
        break;
    case eNorm::NONE:
        break;
    }

    qreal ret = inten_t((num > 0 && den > 0) ? num / den : NAN);
    if (qIsNaN(ret))
        qWarning() << "Bad normalisation value";
    return ret;
}

//! Called only by toCurve(..).
inten_vec Sequence::collectIntens(const Image* intensCorr, const Range& rgeGma) const {
    const Range tthRge = rgeTth();
    const deg tthWdt = tthRge.width();

    const ImageCut& cut = gSession->imageCut();
    const int pixWidth = gSession->imageSize().w - cut.left - cut.right;

    int numBins;
    if (1 < count()) { // combined cluster
        const Measurement* one = first();
        deg delta = one->rgeTth().width() / pixWidth;
        numBins = qCeil(tthWdt / delta);
    } else {
        numBins = pixWidth; // simply match the pixels
    }

    inten_vec intens(numBins, 0);
    vec<int> counts(numBins, 0);

    deg minTth = tthRge.min, deltaTth = tthWdt / numBins;

    for (const Measurement* one : members_)
        one->collectIntens(intensCorr, intens, counts, rgeGma, minTth, deltaTth);

    // sum or average
    if (gSession->intenScaledAvg()) {
        qreal scale = gSession->intenScale();
        for_i (numBins) {
            int cnt = counts.at(i);
            if (cnt > 0)
                intens[i] *= scale / cnt;
        }
    }

    return intens;
}

//! Computes metadata cache md_.
void Sequence::compute_metadata() const {
    const_cast<Sequence*>(this)->md_ = shp_Metadata(new Metadata);
    Metadata* m = const_cast<Metadata*>(md_.data());

    const Metadata& firstMd = *(first()->metadata());

    m->date = firstMd.date;
    m->comment = firstMd.comment;

    // sums: delta mon. count and time,
    // takes the last ones (presumed the maximum) of mon. count and time,
    // averages the rest
    for (const Measurement* one : members_) {
        const Metadata* d = one->metadata().data();
        ASSERT(d);

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
            qWarning() << "decreasing monitor count in combined cluster";
        if (m->time > d->time)
            qWarning() << "decreasing time in combined cluster";
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

// ************************************************************************** //
//  class Cluster
// ************************************************************************** //

Cluster::Cluster(
    const QVector<const Measurement*>& measurements,
    const class Datafile& file, const int index, const int offset)
    : Sequence(measurements)
    , file_(file)
    , index_(index)
    , offset_(offset)
    , activated_(true)
{
}

//! note: the caller must emit sigActivated
void Cluster::setActivated(bool on) {
    if (on==activated_)
        return;
    activated_ = on;
}

int Cluster::totalOffset() const {
    return file_.offset_ + offset();
}

bool Cluster::isIncomplete() const {
    return count()<gSession->dataset().binning();
}

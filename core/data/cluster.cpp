//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "core/session.h"
#include <qmath.h>

//  ***********************************************************************************************
//! @class Sequence

Sequence::Sequence(const QVector<const Measurement*>& measurements)
    : members_(measurements)
{}

//! Returns metadata, averaged over Sequence members. Result is cached.
const Metadata* Sequence::avgeMetadata() const
{
    if (!md_.get()) {
        auto* m = new Metadata { compute_metadata() };
        md_.reset(m);
    }
    return md_.get();
}

#define AVG_ONES(what_function)                 \
    double avg = 0;                             \
    for (const Measurement* one : members_)     \
        avg += one->what_function();            \
    avg /= count();                             \
    return avg;

deg Sequence::omg() const { AVG_ONES(omg) }

deg Sequence::phi() const { AVG_ONES(phi) }

deg Sequence::chi() const { AVG_ONES(chi) }

// combined range of combined cluster
#define RGE_COMBINE(combineOp, what)            \
    Range rge;                                  \
    for (const Measurement* one : members_)     \
        rge.combineOp(one->what);               \
    return rge;

Range Sequence::rgeGma() const { RGE_COMBINE(extendBy, rgeGma()) }

Range Sequence::rgeGmaFull() const { RGE_COMBINE(extendBy, rgeGmaFull()) }

Range Sequence::rgeTth() const { RGE_COMBINE(extendBy, rgeTth()) }

Range Sequence::rgeInten() const { RGE_COMBINE(intersect, rgeInten()) }

double Sequence::avgMonitorCount() const { AVG_ONES(monitorCount) }

double Sequence::avgDeltaMonitorCount() const { AVG_ONES(deltaMonitorCount) }

double Sequence::avgDeltaTime() const { AVG_ONES(deltaTime) }

size2d Sequence::imageSize() const
{
    // all images have the same size; simply take the first one
    return first()->imageSize();
}

Curve Sequence::toCurve() const
{
    return toCurve(rgeGma());
};

Curve Sequence::toCurve(const Range& _rgeGma) const
{
    double _normFactor = normFactor();
    return toCurve(_normFactor, _rgeGma);
};

Curve Sequence::toCurve(double _normFactor) const
{
    return toCurve(_normFactor, rgeGma());
};

Curve Sequence::toCurve(double _normFactor, const Range& _rgeGma) const
{
    QVector<float> intens = collectIntens(_rgeGma);
    int count = intens.count();
    if (!count)
        return {};
    Curve res;
    Range _rgeTth = rgeTth();
    deg minTth = _rgeTth.min;
    deg deltaTth = _rgeTth.width() / count;
    for_i (count)
        res.append(minTth + deltaTth * i, double(intens.at(i) * _normFactor));
    return res;
};

double Sequence::normFactor() const
{
    double num = 1, den = 1;

    switch (gSession->normMode()) {
    case eNorm::MONITOR:
        num = gSession->activeClusters().avgMonitorCount();
        den = avgMonitorCount();
        break;
    case eNorm::DELTA_MONITOR:
        num = gSession->activeClusters().avgDeltaMonitorCount();
        den = avgDeltaMonitorCount();
        break;
    case eNorm::DELTA_TIME:
        num = gSession->activeClusters().avgDeltaTime();
        den = avgDeltaTime();
        break;
    case eNorm::BACKGROUND:
        num = gSession->calcAvgBackground();
        den = gSession->calcAvgBackground(*this);
        break;
    case eNorm::NONE:
        break;
    }

    double ret = float((num > 0 && den > 0) ? num / den : Q_QNAN);
    if (qIsNaN(ret))
        qWarning() << "Bad normalisation value";
    return ret;
}

//! Called only by toCurve(..).
QVector<float> Sequence::collectIntens(const Range& rgeGma) const
{
    const Range tthRge = rgeTth();
    const deg tthWdt = tthRge.width();

    const ImageCut& cut = gSession->imageCut();
    const int pixWidth = gSession->imageSize().w - cut.left() - cut.right();

    int numBins;
    if (1 < count()) { // combined cluster
        const Measurement* one = first();
        deg delta = one->rgeTth().width() / pixWidth;
        numBins = qCeil(tthWdt / delta);
    } else {
        numBins = pixWidth; // simply match the pixels
    }

    QVector<float> intens(numBins, 0);
    QVector<int> counts(numBins, 0);

    deg minTth = tthRge.min, deltaTth = tthWdt / numBins;

    for (const Measurement* one : members_)
        one->collectIntens(intens, counts, rgeGma, minTth, deltaTth);

    // sum or average
    if (gSession->intenScaledAvg()) {
        double scale = gSession->intenScale();
        for_i (numBins) {
            int cnt = counts.at(i);
            if (cnt > 0)
                intens[i] *= scale / cnt;
        }
    }

    return intens;
}

//! Computes metadata cache md_.
Metadata Sequence::compute_metadata() const
{
    Metadata ret;
    const Metadata& firstMd = first()->metadata();
    ret.date = firstMd.date;
    ret.comment = firstMd.comment;

    // sums: delta mon. count and time,
    // takes the last ones (presumed the maximum) of mon. count and time,
    // averages the rest
    for (const Measurement* one : members_) {
        const Metadata& d = one->metadata();

        ret.motorXT += d.motorXT;
        ret.motorYT += d.motorYT;
        ret.motorZT += d.motorZT;

        ret.motorOmg += d.motorOmg;
        ret.motorTth += d.motorTth;
        ret.motorPhi += d.motorPhi;
        ret.motorChi += d.motorChi;

        ret.motorPST += d.motorPST;
        ret.motorSST += d.motorSST;
        ret.motorOMGM += d.motorOMGM;

        ret.nmT += d.nmT;
        ret.nmTeload += d.nmTeload;
        ret.nmTepos += d.nmTepos;
        ret.nmTeext += d.nmTeext;
        ret.nmXe += d.nmXe;
        ret.nmYe += d.nmYe;
        ret.nmZe += d.nmZe;

        ret.deltaMonitorCount += d.deltaMonitorCount;
        ret.deltaTime += d.deltaTime;

        if (ret.monitorCount > d.monitorCount)
            qWarning() << "decreasing monitor count in combined cluster";
        if (ret.time > d.time)
            qWarning() << "decreasing time in combined cluster";
        ret.monitorCount = d.monitorCount;
        ret.time = d.time;
    }

    double fac = 1.0 / count();

    ret.motorXT *= fac;
    ret.motorYT *= fac;
    ret.motorZT *= fac;

    ret.motorOmg *= fac;
    ret.motorTth *= fac;
    ret.motorPhi *= fac;
    ret.motorChi *= fac;

    ret.motorPST *= fac;
    ret.motorSST *= fac;
    ret.motorOMGM *= fac;

    ret.nmT *= fac;
    ret.nmTeload *= fac;
    ret.nmTepos *= fac;
    ret.nmTeext *= fac;
    ret.nmXe *= fac;
    ret.nmYe *= fac;
    ret.nmZe *= fac;

    return ret;
}

//  ***********************************************************************************************
//! @class Cluster

Cluster::Cluster(
    const QVector<const Measurement*>& measurements,
    const class Datafile& file, const int index, const int offset)
    : Sequence(measurements)
    , file_(file)
    , index_(index)
    , offset_(offset)
    , activated_(true)
{}

//! note: the caller must emit sigActivated
void Cluster::setActivated(bool on)
{
    if (on==activated_)
        return;
    activated_ = on;
}

int Cluster::totalOffset() const
{
    return file_.offset_ + offset();
}

bool Cluster::isIncomplete() const
{
    return count()<gSession->dataset().binning();
}

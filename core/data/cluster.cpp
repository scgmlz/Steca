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
#include "core/algo/project_intensity.h"
#include <qmath.h>

//  ***********************************************************************************************
//! @class Sequence

Sequence::Sequence(const QVector<const Measurement*>& measurements)
    : members_(measurements)
    , metadata_(computeAvgeMetadata())
{}

//! Returns metadata, averaged over Sequence members.
Metadata Sequence::computeAvgeMetadata() const
{
    std::vector<const Metadata*> vecMeta;
    for (const Measurement* m : members_)
        vecMeta.push_back(&m->metadata());
    return Metadata::computeAverage(vecMeta);
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
    const Range tthRge = rgeTth();
    const deg tthWdt = tthRge.width();

    const ImageCut& cut = gSession->imageCut();
    const int pixWidth = gSession->imageSize().w - cut.left() - cut.right();

    int numBins;
    if (count()>1) { // combined cluster
        const Measurement* one = first();
        deg delta = one->rgeTth().width() / pixWidth;
        numBins = qCeil(tthWdt / delta);
    } else {
        numBins = pixWidth; // simply match the pixels
    }
    if (!numBins)
        return {};

    QVector<float> intens(numBins, 0);
    QVector<int> counts(numBins, 0);

    deg minTth = tthRge.min, deltaTth = tthWdt / numBins;

    for (const Measurement* one : members_)
        // increment intens and counts
        algo::projectIntensity(intens, counts, *one, _rgeGma, minTth, deltaTth);

    // sum or average
    if (gSession->intenScaledAvg()) {
        double scale = gSession->intenScale();
        for_i (numBins) {
            int cnt = counts.at(i);
            if (cnt > 0)
                intens[i] *= scale / cnt;
        }
    }

    Curve res;
    for_i (numBins)
        res.append(minTth + deltaTth * i, double(intens.at(i) * _normFactor));
    return res;
};

double Sequence::normFactor() const
{
    double num = 1, den = 1; // numerator, denominator

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

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

#include "cluster.h"
#include "core/session.h"
#include "core/algo/collect_intensities.h"
#include "qcr/base/debug.h"
#include <qmath.h>

//  ***********************************************************************************************
//! @class Sequence

Sequence::Sequence(const std::vector<const Measurement*>& measurements)
    : members_(measurements)
    , metadata_(computeAvgMetadata())
{}

//! Returns metadata, averaged over Sequence members.
Metadata Sequence::computeAvgMetadata() const
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

double Sequence::avgTime() const { AVG_ONES(time) }

double Sequence::avgDeltaTime() const { AVG_ONES(deltaTime) }

size2d Sequence::imageSize() const
{
    // all images have the same size; simply take the first one
    return first()->imageSize();
}

double Sequence::normFactor() const
{
    double num = 1, den = 1; // numerator, denominator
    const ActiveClusters& seq = gSession->activeClusters();

    switch (gSession->normMode()) {
    case eNorm::MONITOR:
        num = seq.avgMonitorCount();
        den = avgMonitorCount();
        break;
    case eNorm::DELTA_MONITOR:
        num = seq.avgDeltaMonitorCount();
        den = avgDeltaMonitorCount();
        break;
    case eNorm::TIME:
        num = seq.avgTime();
        den = avgTime();
        break;
    case eNorm::DELTA_TIME:
        num = seq.avgDeltaTime();
        den = avgDeltaTime();
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
//! @class GammaSector

void GammaSector::init()
{
    QObject::connect(gSession, &Session::sigDetector, [this]() { cachedDfgram_.invalidate(); });
}

Dfgram recomputeSectorDfgram(const GammaSector* const gSector)
{
    // qDebug() << "recompute dfgram" << gSector->owningCluster_->index() << "for sector i,n =" << gSector->i_ << gSector->n_;
    return gSector->owningCluster_->segmentalDfgram(gSector->i_);
}

//  ***********************************************************************************************
//! @class Cluster

Cluster::Cluster(
    const std::vector<const Measurement*>& measurements,
    const class Datafile& file, const int index, const int offset)
    : Sequence(measurements)
    , gSectors(this, []()->int{return gSession->gammaSelection().numSlices.val();})
    , file_(file)
    , index_(index)
    , offset_(offset)
    , activated_(true)
{
}

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
    return count() < gSession->dataset().binning.val();
}

Dfgram Cluster::segmentalDfgram(int i) const
{
    return Dfgram(algo::projectCluster(*this, rgeGma().slice(i,gSectors.size())));
}

GammaSector& Cluster::currentGammaSector() const
{
    return gSectors.get(gSession->gammaSelection().currSlice.val()-1);
}

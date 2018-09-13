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

#include "core/data/cluster.h"
#include "core/data/from_map.h"
#include "core/session.h"
#include "core/data/collect_intensities.h"
#include "qcr/base/debug.h" // warning

//  ***********************************************************************************************
//! @class Sequence

Sequence::Sequence(const std::vector<const Measurement*>& measurements)
    : members_(measurements)
    , metadata_(computeAvgMetadata())
{}

Range Sequence::rgeGma() const {
    Range ret;
    for (const Measurement* m : members_)
        ret.extendBy(fromMap::rgeGma(m));
    return ret;
}

Range Sequence::rgeGmaFull() const {
    Range ret;
    for (const Measurement* m : members_)
        ret.extendBy(fromMap::rgeGmaFull(m));
    return ret;
}

Range Sequence::rgeTth() const {
    Range ret;
    for (const Measurement* m : members_)
        ret.extendBy(fromMap::rgeTth(m));
    return ret;
}

Range Sequence::rgeInten() const {
    Range ret;
    for (const Measurement* m : members_)
        ret.intersect(m->rgeInten());
    return ret;
}

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
    avg /= size();                             \
    return avg;

deg Sequence::omg() const { AVG_ONES(omg) }

deg Sequence::phi() const { AVG_ONES(phi) }

deg Sequence::chi() const { AVG_ONES(chi) }

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

    switch ((eNorm)gSession->params.howtoNormalize.val()) {
    case eNorm::MONITOR:
        num = gSession->activeClusters.grandAvgMonitorCount.get();
        den = avgMonitorCount();
        break;
    case eNorm::DELTA_MONITOR:
        num = gSession->activeClusters.grandAvgDeltaMonitorCount.get();
        den = avgDeltaMonitorCount();
        break;
    case eNorm::TIME:
        num = gSession->activeClusters.grandAvgTime.get();
        den = avgTime();
        break;
    case eNorm::DELTA_TIME:
        num = gSession->activeClusters.grandAvgDeltaTime.get();
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
//! @class Cluster

namespace {
Dfgram computeSectorDfgram(const Cluster* const parent, const int jS)
{
    int nS = gSession->gammaSelection.numSlices.val();
    return Dfgram(algo::projectCluster(*parent, parent->rgeGma().slice(jS,nS)));
}
} //namespace

Cluster::Cluster(
    const std::vector<const Measurement*>& measurements,
    const class Datafile& file, const int index, const int offset)
    : Sequence(measurements)
    , dfgrams([]()->int{return gSession->gammaSelection.numSlices.val();},
              [](const Cluster* parent, int jS)->Dfgram{
                  return computeSectorDfgram(parent, jS); })
    , file_(file)
    , index_(index)
    , offset_(offset)
    , activated_(true)
{}

int Cluster::totalOffset() const
{
    return file_.offset_ + offset();
}

bool Cluster::isIncomplete() const
{
    return size() < gSession->dataset.binning.val();
}

const Dfgram& Cluster::currentDfgram() const
{
    return dfgrams.getget(this, gSession->gammaSelection.currSlice.val()-1);
}

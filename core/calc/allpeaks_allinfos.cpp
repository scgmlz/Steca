//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/allpeaks_allinfos.cpp
//! @brief     Implements class AllInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/calc/allpeaks_allinfos.h"
#include "core/base/async.h"
#include "core/fitengine/double_with_error.h"
#include "core/calc/coord_trafos.h"
#include "core/calc/interpolate_polefig.h"
#include "core/peakfit/peak_function.h"
#include "core/typ/mapped.h"
#include "core/session.h"
#include "qcr/base/debug.h" // ASSERT

namespace {

//! Fits peak to the given gamma gRange and constructs a PeakInfo.
PeakInfo getPeak(int jP, const Cluster& cluster, int iGamma)
{
    const OnePeakSettings& settings = gSession->peaksSettings.at(jP);
    const Range& fitrange = settings.range();
    const Metadata* metadata = &cluster.avgMetadata();
    const Range gRange = gSession->gammaSelection.slice2range(cluster.rangeGma(), iGamma);
    deg alpha, beta;
    // TODO/math use fitted tth center, not center of given fit range
    algo::calculateAlphaBeta(alpha, beta, fitrange.center(), gRange.center(),
                             cluster.chi(), cluster.omg(), cluster.phi());
    if (fitrange.isEmpty())
        qFatal("why would the fit range be empty??");
        // return PeakInfo{metadata, alpha, beta, gRange};

    const Dfgram& dfgram = cluster.dfgrams.yield_at(iGamma, &cluster);

    Mapped out;
    if (settings.isRaw()) {
        out = dfgram.getRawOutcome(jP);
    } else {
        const Fitted& pFct = dfgram.getPeakFit(jP);
        const PeakFunction*const peakFit = dynamic_cast<const PeakFunction*>(pFct.fitFunction());
        ASSERT(peakFit);
        const Mapped& po = peakFit->outcome(pFct);
        if (po.has("center") && fitrange.contains(po.at<double>("center")))
            out = po;
    }
    out.set("alpha", alpha);
    out.set("beta", beta);
    out.set("gamma_min", gRange.min);
    out.set("gamma_max", gRange.max);
    return PeakInfo{metadata, out};
}

OnePeakAllInfos computeDirectInfoSequence(int jP)
{
    TakesLongTime progress{"peak fitting", gSession->activeClusters.size()};
    OnePeakAllInfos ret;
    int nGamma = qMax(1, gSession->gammaSelection.numSlices.val());
    for (const Cluster* cluster : gSession->activeClusters.clusters.yield()) {
        progress.step();
        for (int i=0; i<nGamma; ++i) {
            PeakInfo refInfo = getPeak(jP, *cluster, i);
            if (refInfo.has("intensity"))
                ret.appendPeak(std::move(refInfo));
        }
    }
    return ret;
}

} // namespace


AllPeaksAllInfos::AllPeaksAllInfos()
    : direct {[]()->int{return gSession->peaksSettings.size();},
        [](int jP, const AllPeaksAllInfos*)->OnePeakAllInfos{
            return computeDirectInfoSequence(jP); }}
    , interpolated {[]()->int{return gSession->peaksSettings.size();},
        [](int jP, const AllPeaksAllInfos* parent)->OnePeakAllInfos{
            return algo::interpolateInfos(parent->direct.yield_at(jP,parent)); }}
{}

void AllPeaksAllInfos::invalidateAll() const
{
    direct.clear_vector();
    invalidateInterpolated();
}

void AllPeaksAllInfos::invalidateInterpolated() const
{
    interpolated.clear_vector();
}

void AllPeaksAllInfos::invalidateAt(int jP) const
{
    direct      .invalidate_at(jP);
    interpolated.invalidate_at(jP);
}

const OnePeakAllInfos* AllPeaksAllInfos::currentDirect() const
{
    if (!gSession->peaksSettings.size())
        return nullptr;
    ASSERT(direct.size(this)==gSession->peaksSettings.size());
    int jP = gSession->peaksSettings.selectedIndex();
    return &direct.yield_at(jP,this);
}

const OnePeakAllInfos* AllPeaksAllInfos::currentInterpolated() const
{
    if (!gSession->peaksSettings.size())
        return nullptr;
    ASSERT(interpolated.size(this)==gSession->peaksSettings.size());
    int jP = gSession->peaksSettings.selectedIndex();
    return &interpolated.yield_at(jP,this);
}

const OnePeakAllInfos* AllPeaksAllInfos::currentInfoSequence() const
{
    return gSession->params.interpolParams.enabled.val() ? currentInterpolated() : currentDirect();
}

const OnePeakAllInfos* AllPeaksAllInfos::At(int jP) const
{
    return gSession->params.interpolParams.enabled.val() ?
        &interpolated.yield_at(jP,this) : &direct.yield_at(jP,this);
}

const std::vector<const OnePeakAllInfos*> AllPeaksAllInfos::allInterpolated() const
{
    std::vector<const OnePeakAllInfos*> ret;
    for (const auto & v : interpolated.vecRef())
        ret.push_back(&v.yield(this));
    return ret;
}

const std::vector<const OnePeakAllInfos*> AllPeaksAllInfos::allDirect() const
{
    std::vector<const OnePeakAllInfos*> ret;
    for (const auto & v : direct.vecRef())
        ret.push_back(&v.yield(this));
    return ret;

}

const std::vector<const OnePeakAllInfos*> AllPeaksAllInfos::allInfoSequences() const
{
    return gSession->params.interpolParams.enabled.val() ?
                allInterpolated() : allDirect();
}

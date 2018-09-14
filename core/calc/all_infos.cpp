//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/all_infos.cpp
//! @brief     Implements class AllInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/calc/all_infos.h"
#include "core/misc/async.h"
#include "core/calc/coord_trafos.h"
#include "core/calc/interpolate_polefig.h"
#include "core/fit/peak_function.h"
#include "core/session.h"
#include "qcr/base/debug.h" // ASSERT

namespace {

//! Fits peak to the given gamma gRange and constructs a PeakInfo.
PeakInfo getPeak(int jP, const Cluster& cluster, int iGamma)
{
    const Peak& peak = gSession->peaks.at(jP);
    const Range& fitrange = peak.range();
    const Metadata* metadata = &cluster.avgMetadata();
    const Range gRange = gSession->gammaSelection.slice2range(iGamma);
    deg alpha, beta;
    // TODO/math use fitted tth center, not center of given fit range
    algo::calculateAlphaBeta(alpha, beta, fitrange.center(), gRange.center(),
                             cluster.chi(), cluster.omg(), cluster.phi());
    if (fitrange.isEmpty())
        return {metadata, alpha, beta, gRange};

    const Dfgram& dfgram = cluster.dfgrams.getget(&cluster, iGamma);

    // TODO: the following could be simplified if RawOutcome were replaced by PeakOutcome
    std::unique_ptr<DoubleWithError> center;
    std::unique_ptr<DoubleWithError> fwhm;
    std::unique_ptr<DoubleWithError> intensity;
    if (peak.isRaw()) {
        const RawOutcome& out = dfgram.getRawOutcome(jP);
        center    .reset(new DoubleWithError{out.getCenter(),0});
        fwhm      .reset(new DoubleWithError{out.getFwhm(),0});
        intensity .reset(new DoubleWithError{out.getIntensity(),0});
    } else {
        const Fitted& pFct = dfgram.getPeakFit(jP);
        const auto* peakFit = dynamic_cast<const PeakFunction*>(pFct.f);
        ASSERT(peakFit);
        const PeakOutcome out = peakFit->outcome(pFct);
        center    .reset(new DoubleWithError{out.center});
        fwhm      .reset(new DoubleWithError{out.fwhm});
        intensity .reset(new DoubleWithError{out.intensity});
    }

    if (!fitrange.contains(center->value)) // TODO/math generalize to fitIsCredible
        return {metadata, alpha, beta, gRange};

    // TODO pass PeakOutcome instead of 6 components
    return {metadata, alpha, beta, gRange, intensity->value, intensity->error,
            deg(center->value), deg(center->error), fwhm->value, fwhm->error};
}

InfoSequence computeDirectInfoSequence(int jP)
{
    TakesLongTime progress("peak fitting", gSession->activeClusters.size());
    InfoSequence ret;
    int nGamma = qMax(1, gSession->gammaSelection.numSlices.val());
    for (const Cluster* cluster : gSession->activeClusters.clusters.get()) {
        progress.step();
        for (int i=0; i<nGamma; ++i) {
            PeakInfo refInfo = getPeak(jP, *cluster, i);
            if (!qIsNaN(refInfo.inten()))
                ret.appendPeak(std::move(refInfo));
        }
    }
    return ret;
}

} // namespace


AllInfos::AllInfos()
    : direct {[]()->int{return gSession->peaks.size();},
        [](const AllInfos*, int jP)->InfoSequence{
            return computeDirectInfoSequence(jP); }}
    , interpolated {[]()->int{return gSession->peaks.size();},
        [](const AllInfos* parent, int jP)->InfoSequence{
            return algo::interpolateInfos(parent->direct.getget(parent,jP)); }}
{}

void AllInfos::invalidateAll() const
{
    direct      .invalidate();
    interpolated.invalidate();
}

void AllInfos::invalidateAt(int jP) const
{
    direct      .get(this,jP).invalidate();
    interpolated.get(this,jP).invalidate();
}

void AllInfos::invalidateInterpolated() const
{
    interpolated.invalidate();
}

const InfoSequence* AllInfos::currentDirect() const
{
    if (!gSession->peaks.size())
        return nullptr;
    ASSERT(direct.size(this)==gSession->peaks.size());
    int jP = gSession->peaks.selectedIndex();
    return &direct.getget(this,jP);
}

const InfoSequence* AllInfos::currentInterpolated() const
{
    if (!gSession->peaks.size())
        return nullptr;
    ASSERT(interpolated.size(this)==gSession->peaks.size());
    int jP = gSession->peaks.selectedIndex();
    return &interpolated.getget(this,jP);
}

const InfoSequence* AllInfos::currentInfoSequence() const
{
    return gSession->params.interpolParams.enabled.val() ? currentInterpolated() : currentDirect();
}

const std::vector<InfoSequence const *> AllInfos::allInterpolated() const
{
    std::vector<InfoSequence const *> ret;
    for (const auto & v : interpolated.data())
        ret.push_back(&v.get(this));
    return ret;
}

const std::vector<InfoSequence const *> AllInfos::allDirect() const
{
    std::vector<InfoSequence const *> ret;
    for (const auto & v : direct.data())
        ret.push_back(&v.get(this));
    return ret;

}

const std::vector<InfoSequence const *> AllInfos::allInfoSequences() const
{
    return gSession->params.interpolParams.enabled.val() ?
                allInterpolated() : allDirect();
}

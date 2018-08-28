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
#include "core/aux/async.h"
#include "core/calc/coord_trafos.h"
#include "core/calc/interpolate_polefig.h"
#include "core/fit/peak_function.h"
#include "core/session.h"
#include "qcr/base/debug.h" // ASSERT

namespace {

//! Fits peak to the given gamma gRange and constructs a PeakInfo.
Optional<PeakInfo> getPeak(int jP, const Cluster& cluster, int iGamma)
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
        return PeakInfo{metadata, alpha, beta, gRange};

    const Dfgram& dfgram = cluster.dfgrams.getget(&cluster, iGamma);

    // TODO: the following could be simplified if RawOutcome were replaced by PeakOutcome
    Optional<DoubleWithError> center;
    Optional<DoubleWithError> fwhm;
    Optional<DoubleWithError> intensity;
    if (peak.isRaw()) {
        const Optional<RawOutcome>& out = dfgram.getRawOutcome(jP);
        center    = APPLY(out, v, { return DoubleWithError{v.getCenter(), 0}; });
        fwhm      = APPLY(out, v, { return DoubleWithError{v.getFwhm(), 0}; });
        intensity = APPLY(out, v, { return DoubleWithError{v.getIntensity(), 0}; });
    } else {
        const Optional<Fitted>& pFct = dfgram.getPeakFit(jP);
        const auto peakFit = dynamic_optional_cast<const PeakFunction>(GET_SAFE(pFct,f));
        //ASSERT(peakFit);
        const Optional<PeakOutcome> out = APPLY(pFct, pFct, { return CALLP(peakFit, outcome, pFct); });
        center    = GET_SAFE(out, center);
        fwhm      = GET_SAFE(out, fwhm);
        intensity = GET_SAFE(out, intensity);
    }

    if (center.isEmpty() || fwhm.isEmpty() || intensity.isEmpty())
        return emptyOptional;
    // center, fwhm and intensity must have a value from here on...

    if (!fitrange.contains(center->value)) // TODO/math generalize to fitIsCredible
        return PeakInfo{metadata, alpha, beta, gRange};

    // TODO pass PeakOutcome instead of 6 components
    return PeakInfo{metadata, alpha, beta, gRange, intensity->value, intensity->error,
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
            APPLY(getPeak(jP, *cluster, i), refInfo, {
                if (!qIsNaN(refInfo.inten()))
                    ret.appendPeak(PeakInfo(refInfo));
            });
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
    return gSession->params.interpolParams.enabled.val() ?
        currentInterpolated() : currentDirect();
}

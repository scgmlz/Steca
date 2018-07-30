//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/algo/fitting.cpp
//! @brief     Implements function rawFit, rawFits
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "fitting.h"
#include "core/algo/coord_trafos.h"
#include "core/data/cluster.h"
#include "core/def/idiomatic_for.h"
#include "core/session.h"
#include "core/typ/async.h"

namespace {

//! Fits peak to the given gamma sector and constructs a PeakInfo.
PeakInfo rawFit(Cluster& cluster, int iGamma, Peak& peak)
{
    std::unique_ptr<PeakFunction> peakFunction( FunctionRegistry::clone(peak.peakFunction()) );
    const Range& fitrange = peakFunction->fitRange();
    const Metadata* metadata = &cluster.avgMetadata();
    const Range gammaSector = gSession->gammaSelection().slice2range(iGamma);
    deg alpha, beta;
    // TODO (MATH) use fitted tth center, not center of given fit range
    algo::calculateAlphaBeta(alpha, beta, fitrange.center(), gammaSector.center(),
                             cluster.chi(), cluster.omg(), cluster.phi());
    if (fitrange.isEmpty())
        return {metadata, alpha, beta, gammaSector};

    auto& baseline = gSession->baseline();

    // Diffractogram minus fitted background:
    const Curve& curve = {}; // TODO NOW cluster.gSector.get(iGamma)...
    const Polynom f = Polynom::fromFit(baseline.polynomDegree.val(), curve, baseline.ranges());
    const Curve curve2 = curve.subtract([f](double x) {return f.y(x);});

    // Fit the peak:
    peak.fit(curve2);
    qpair fitresult = peakFunction->fittedPeak();
    if (!fitrange.contains(fitresult.x))
        return {metadata, alpha, beta, gammaSector};

    float fwhm = peakFunction->fittedFWHM();
    qpair peakError = peakFunction->peakError();
    float fwhmError = peakFunction->fwhmError();
    return {metadata, alpha, beta, gammaSector, float(fitresult.y), float(peakError.y),
            deg(fitresult.x), deg(peakError.x), float(fwhm), float(fwhmError)};
}

} // namespace


//! Gathers PeakInfos from Datasets.

//! Either uses the whole gamma range of the cluster (if gammaSector is invalid),
//!  or user limits the range.
//! Even though the betaStep of the equidistant polefigure grid is needed here,
//!  the returned infos won't be on the grid.
//! TODO? gammaStep separately?

void algo::rawFits(class QProgressBar* progressBar)
{
    if (!gSession->peaks().count())
        qFatal("algo::rawFits called before peak is defined");
    Peak* peak = gSession->peaks().selectedPeak();
    if (!peak)
        qFatal("algo::rawFits called while no peak is selected");

    PeakInfos tmp;
    const ActiveClusters& seq = gSession->activeClusters();
    Progress progress(progressBar, "peak fitting", seq.size());
    int nGamma = qMax(1, gSession->gammaSelection().numSlices.val());
    for (Cluster* cluster : seq.clusters()) {
        progress.step();
        for_i (nGamma) {
            PeakInfo refInfo = rawFit(*cluster, i, *peak);
            if (!qIsNaN(refInfo.inten()))
                tmp.append(std::move(refInfo));
        }
    }
    gSession->setDirectPeakInfos(std::move(tmp));
}

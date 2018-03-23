// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/peak_info.cpp
//! @brief     Implements classes PeakInfo, PeakInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/calc/peak_info.h"
#include "core/def/idiomatic_for.h"
#include "core/typ/async.h"
#include "core/session.h"

// ************************************************************************** //
//  class PeakInfo
// ************************************************************************** //

/* NOTE Invalid output parameters are set to NaNs. However, some analysis
 * programs
 * debug::ensure -1 as unknown value; thus, NaN parameter values should be output
 * as -1 when output is written for these programs (polefigure!).
 */

PeakInfo::PeakInfo()
    : PeakInfo(
          shp_Metadata(),
          NAN, NAN, Range(), inten_t(NAN), inten_t(NAN), deg(NAN), deg(NAN), NAN, NAN)
{}

PeakInfo::PeakInfo(
    shp_Metadata md,
    deg alpha, deg beta, Range rgeGma, inten_t inten, inten_t intenError,
    deg tth, deg tthError, fwhm_t fwhm, fwhm_t fwhmError)
    : md_(md)
    , alpha_(alpha)
    , beta_(beta)
    , rgeGma_(rgeGma)
    , inten_(inten)
    , intenError_(intenError)
    , tth_(tth)
    , tthError_(tthError)
    , fwhm_(fwhm)
    , fwhmError_(fwhmError)
{}

PeakInfo::PeakInfo(
    shp_Metadata md, deg alpha, deg beta, Range rgeGma)
    : PeakInfo(
        md, alpha, beta, rgeGma, inten_t(NAN), inten_t(NAN), deg(NAN), deg(NAN), fwhm_t(NAN),
        fwhm_t(NAN))
{}

PeakInfo::PeakInfo(
    deg alpha, deg beta, Range rgeGma, inten_t inten, inten_t intenError, deg tth,
    deg tthError, fwhm_t fwhm, fwhm_t fwhmError)
    : PeakInfo(
        shp_Metadata(),
        alpha, beta, rgeGma, inten, intenError, tth, tthError, fwhm, fwhmError)
{}

PeakInfo::PeakInfo(deg alpha, deg beta)
    : PeakInfo(
          alpha, beta, Range(), inten_t(NAN), inten_t(NAN), deg(NAN), deg(NAN), fwhm_t(NAN),
          fwhm_t(NAN))
{}

//! Fits peak to the given gamma sector and constructs a PeakInfo.
PeakInfo::PeakInfo(const Cluster* cluster, const Peak& peak, const Range& gmaSector)
{
    // fit peak, and retrieve peak parameters:
    Curve curve = cluster->toCurve(gmaSector);
    auto& baseline = gSession->baseline();
    const Polynom f = Polynom::fromFit(baseline.polynomDegree(), curve, baseline.ranges());
    curve.subtract([f](qreal x) {return f.y(x);});

    std::unique_ptr<PeakFunction> peakFunction( FunctionRegistry::clone(peak.peakFunction()) );
    peakFunction->fit(curve);
    const Range& rgeTth = peakFunction->range();
    qpair fitresult = peakFunction->fittedPeak();
    fwhm_t fwhm = peakFunction->fittedFWHM();
    qpair peakError = peakFunction->peakError();
    fwhm_t fwhmError = peakFunction->fwhmError();

    // compute alpha, beta:
    deg alpha, beta;
    cluster->calculateAlphaBeta(rgeTth.center(), gmaSector.center(), alpha, beta);

    shp_Metadata metadata = cluster->avgeMetadata();

    *this = rgeTth.contains(fitresult.x)
        ? PeakInfo(
              metadata, alpha, beta, gmaSector, inten_t(fitresult.y), inten_t(peakError.y),
              deg(fitresult.x), deg(peakError.x), fwhm_t(fwhm), fwhm_t(fwhmError))
        : PeakInfo(metadata, alpha, beta, gmaSector);
}

QStringList PeakInfo::dataTags(bool out) {
    QStringList tags;
    for_i (int(eReflAttr::NUM_REFL_ATTR))
        tags.append(reflStringTag(i, out));
    tags.append(Metadata::attributeTags(out));
    return tags;
}

cmp_vec PeakInfo::dataCmps() {
    static cmp_vec cmps;
    if (cmps.isEmpty()) {
        cmps = cmp_vec{ cmp_real, cmp_real, cmp_real, cmp_real, cmp_real,
                        cmp_real, cmp_real, cmp_real, cmp_real, cmp_real };
        cmps.append(Metadata::attributeCmps());
    }
    return cmps;
}

row_t PeakInfo::data() const {
    row_t row{ QVariant(alpha()),      QVariant(beta()),     QVariant(rgeGma().min),
                    QVariant(rgeGma().max), QVariant(inten()),    QVariant(intenError()),
                    QVariant(tth()),        QVariant(tthError()), QVariant(fwhm()),
                    QVariant(fwhmError()) };

    row.append(md_ ? md_->attributeValues() : Metadata::attributeNaNs());
    return row;
}

QString const PeakInfo::reflStringTag(int attr, bool out) {
    switch (eReflAttr(attr)) {
    case eReflAttr::ALPHA: return out ? "alpha" : "α";
    case eReflAttr::BETA: return out ? "beta" : "β";
    case eReflAttr::GAMMA1: return out ? "gamma1" : "γ1";
    case eReflAttr::GAMMA2: return out ? "gamma2" : "γ2";
    case eReflAttr::INTEN: return "inten";
    case eReflAttr::SIGMA_INTEN: return out ? "sinten" : "σinten";
    case eReflAttr::TTH: return out ? "2theta" : "2θ";
    case eReflAttr::SIGMA_TTH: return out ? "s2theta" : "σ2θ";
    case eReflAttr::FWHM: return "fwhm";
    case eReflAttr::SIGMA_FWHM: return out ? "sfwhm" : "σfwhm";
    }
    qFatal("impossible case");
}


// ************************************************************************** //
//  class PeakInfos
// ************************************************************************** //

//! Gathers PeakInfos from Datasets.

//! Either uses the whole gamma range of the cluster (if gammaSector is invalid),
//!  or user limits the range.
//! Even though the betaStep of the equidistant polefigure grid is needed here,
//!  the returned infos won't be on the grid.
//! TODO? gammaStep separately?

PeakInfos::PeakInfos(const Peak& peak, Progress* progress)
{
    if (progress)
        progress->setTotal(gSession->activeClusters().size());
    int nGamma = qMax(1, gSession->gammaSelection().numSlices());
    for (const Cluster* cluster : gSession->activeClusters().clusters()) {
        if (progress)
            progress->step();
        for_i (nGamma) {
            const PeakInfo refInfo {cluster, peak, gSession->gammaSelection().slice2range(i)};
            if (!qIsNaN(refInfo.inten()))
                this->append(refInfo);
        }
    }
}


void PeakInfos::append(const PeakInfo& info) {
    QVector<PeakInfo>::append(info);
    invalidate();
}

inten_t PeakInfos::averageInten() const {
    if (qIsNaN(avgInten_)) {
        avgInten_ = 0;
        int cnt = 0;
        for (auto& info : *this) {
            qreal inten = info.inten();
            if (qIsFinite(inten)) {
                avgInten_ += inten;
                ++cnt;
            }
        }
        if (cnt)
            avgInten_ /= cnt;
    }
    return avgInten_;
}

const Range& PeakInfos::rgeInten() const {
    if (!rgeInten_.isValid()) {
        for_i (count())
            rgeInten_.extendBy(at(i).inten());
    }
    return rgeInten_;
}

void PeakInfos::invalidate() {
    avgInten_ = inten_t(NAN);
    rgeInten_.invalidate();
}

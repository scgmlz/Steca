//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "core/calc/peak_info.h"
#include "core/aux/async.h"
#include "core/calc/coord_trafos.h"
#include "core/calc/interpolate_polefig.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

namespace {

// sorts xs and ys the same way, by (x,y)
static void sortColumns(std::vector<double>& xs, std::vector<double>& ys, std::vector<int>& is)
{
    ASSERT(xs.size() == ys.size());

    int count = xs.size();

    is.resize(count);
    for (int i=0; i<count; ++i)
        is[i] = i;

    std::sort(is.begin(), is.end(), [&xs, &ys](int i1, int i2) {
        double x1 = xs.at(i1), x2 = xs.at(i2);
        if (x1 < x2)
            return true;
        if (x1 > x2)
            return false;
        return ys.at(i1) < ys.at(i2); });

    std::vector<double> r(count);

    for (int i=0; i<count; ++i)
        r[i] = xs.at(is.at(i));
    xs = r;

    for (int i=0; i<count; ++i)
        r[i] = ys.at(is.at(i));
    ys = r;
}

} // namespace


//  ***********************************************************************************************
//! @class PeakInfo
//!
//! Invalid output parameters are set to NaNs. However, some analysis programs
//! debug::ensure -1 as unknown value; thus, NaN parameter values should be output
//! as -1 when output is written for these programs (polefigure!).

PeakInfo::PeakInfo(const Metadata* md, deg alpha, deg beta, Range rgeGma, double inten,
                   double intenError, deg tth, deg tthError, double fwhm, double fwhmError)
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

PeakInfo::PeakInfo(const Metadata* md, deg alpha, deg beta, Range rgeGma)
    : PeakInfo(md, alpha, beta, rgeGma, Q_QNAN, Q_QNAN, deg(Q_QNAN), deg(Q_QNAN), Q_QNAN, Q_QNAN)
{}

PeakInfo::PeakInfo(deg alpha, deg beta, Range rgeGma, double inten, double intenError, deg tth,
                   deg tthError, double fwhm, double fwhmError)
    : PeakInfo(nullptr, alpha, beta, rgeGma, inten, intenError, tth, tthError, fwhm, fwhmError)
{}

PeakInfo::PeakInfo(deg alpha, deg beta)
    : PeakInfo(alpha, beta, Range(), Q_QNAN, Q_QNAN, deg(Q_QNAN), deg(Q_QNAN), Q_QNAN, Q_QNAN)
{}

QStringList PeakInfo::dataTags(bool out)
{
    QStringList ret;
    for (int i=0; i<int(eReflAttr::NUM_REFL_ATTR); ++i)
        ret.append(reflStringTag(i, out));
    ret.append(Metadata::attributeTags(out));
    return ret;
}

QStringList PeakInfo::metaTags() // TODO simplify
{
    QStringList ret = dataTags(false);
    for (int i=0; i< (Metadata::numAttributes(false) - Metadata::numAttributes(true)); ++i)
        ret.removeLast(); // remove all tags that are not numbers
    return ret;
}

//! For use in Bigtable.

std::vector<VariantComparator*> PeakInfo::dataCmps()
{
    static std::vector<VariantComparator*> ret;
    if (ret.empty()) {
        ret = std::vector<VariantComparator*>{ cmp_real, cmp_real, cmp_real, cmp_real, cmp_real,
                        cmp_real, cmp_real, cmp_real, cmp_real, cmp_real };
        for (auto* cmp: Metadata::attributeCmps())
            ret.push_back(cmp);
    }
    return ret;
}

std::vector<QVariant> PeakInfo::data() const
{
    std::vector<QVariant> ret{
        QVariant(alpha()),      QVariant(beta()),     QVariant(rgeGma().min),
            QVariant(rgeGma().max), QVariant(inten()),    QVariant(intenError()),
            QVariant(tth()),        QVariant(tthError()), QVariant(fwhm()),
            QVariant(fwhmError()) };
    auto values_to_append = md_ ? md_->attributeValues() : Metadata::attributeNaNs();
    ret.insert(ret.end(), values_to_append.begin(), values_to_append.end());
    return ret;
}

QString const PeakInfo::reflStringTag(int attr, bool out)
{
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
    default: ;
    }
    qFatal("impossible case");
}


//  ***********************************************************************************************
//! @class PeakInfos

void PeakInfos::appendPeak(PeakInfo&& info)
{
    peaks_.push_back(std::move(info));
}

//! Returns entries idxX and idxY, as sorted vectors X and Ylow,Y,Yhig, for use in diagrams.

void PeakInfos::get4(const int idxX, const int idxY,
                     std::vector<double>& xs, std::vector<double>& ys,
                     std::vector<double>& ysLow, std::vector<double>& ysHig) const
{
    int n = peaks_.size();
    xs.resize(n);
    ys.resize(n);

    for (int i=0; i<n; ++i) {
        const std::vector<QVariant> row = peaks_.at(i).data();
        xs[i] = row.at(idxX).toDouble();
        ys[i] = row.at(idxY).toDouble();
    }

    std::vector<int> is;
    sortColumns(xs, ys, is);

    using eReflAttr = PeakInfo::eReflAttr;
    eReflAttr ye = (eReflAttr) idxY;
    const Peak* peak = gSession->peaks.selectedPeak();
    if (peak
        && !peak->isRaw()
        && (ye==eReflAttr::INTEN || ye==eReflAttr::TTH || ye==eReflAttr::FWHM)) {
        ysLow.resize(n);
        ysHig.resize(n);
        for (int i=0; i<n; ++i) {
            const std::vector<QVariant> row = peaks_.at(is.at(i)).data();
            double sigma = row.at(idxY+1).toDouble(); // SIGMA_X has tag position of X plus 1
            double y = ys.at(i);
            ysLow[i] = y - sigma;
            ysHig[i] = y + sigma;
        }
    } else {
        ysLow.resize(0);
        ysHig.resize(0);
    }
}

//! For debugging only.

void PeakInfos::inspect(const QString& header) const
{
    qDebug() << header << "#peaks=" << peaks_.size();
    if (!peaks_.size())
        return;
    const PeakInfo& p = peaks_.front();
    qDebug() << " .. first entry: alpha=" << p.alpha() << "beta=" << p.beta()
             << "inten=" << p.inten();
    const PeakInfo& q = peaks_.back();
    qDebug() << " ..  last entry: alpha=" << q.alpha() << "beta=" << q.beta()
             << "inten=" << q.inten();
}

//  ***********************************************************************************************
//! @class AllPeaks

namespace {

//! Fits peak to the given gamma gRange and constructs a PeakInfo.
PeakInfo getPeak(int jP, const Cluster& cluster, int iGamma)
{
    const Range& fitrange = gSession->peaks.at(jP).range();
    const Metadata* metadata = &cluster.avgMetadata();
    const Range gRange = gSession->gammaSelection.slice2range(iGamma);
    deg alpha, beta;
    // TODO (MATH) use fitted tth center, not center of given fit range
    algo::calculateAlphaBeta(alpha, beta, fitrange.center(), gRange.center(),
                             cluster.chi(), cluster.omg(), cluster.phi());
    if (fitrange.isEmpty())
        return {metadata, alpha, beta, gRange};

    const Dfgram& dfgram = cluster.dfgrams.getget(&cluster, iGamma);
    const PeakFunction& peakFit = dfgram.getPeakFit(jP); // TODO NOW get dfgram

    FitParameter center    = peakFit.getCenter();
    FitParameter fwhm      = peakFit.getFwhm();
    FitParameter intensity = peakFit.getIntensity();

    if (!fitrange.contains(center.value())) // TODO generalize to fitIsCredible
        return {metadata, alpha, beta, gRange};

    return {metadata, alpha, beta, gRange, intensity.value(), intensity.error(),
            deg(center.value()), deg(center.error()), fwhm.value(), fwhm.error()};
}

PeakInfos computeDirectPeakInfos(int jP)
{
    TakesLongTime progress("peak fitting", gSession->activeClusters.size());
    PeakInfos ret;
    int nGamma = qMax(1, gSession->gammaSelection.numSlices.val()); // TODO ensure >0 in GSelection
    for (const Cluster* cluster : gSession->activeClusters.clusters.get()) {
        progress.step();
        for (int i=0; i<nGamma; ++i) {
            PeakInfo refInfo = getPeak(jP, *cluster, i);
            if (!qIsNaN(refInfo.inten()))
                ret.appendPeak(std::move(refInfo));
        }
    }
    ret.inspect("computeDirectPeakInfos");
    return ret;
}

} // namespace

AllPeaks::AllPeaks()
    : direct {[]()->int{return gSession->peaks.count();},
        [](const AllPeaks*, int jP)->PeakInfos{
            return computeDirectPeakInfos(jP); }}
    , interpolated {[]()->int{return gSession->peaks.count();},
        [](const AllPeaks* parent, int jP)->PeakInfos{
            return algo::interpolateInfos(parent->direct.getget(parent,jP)); }}
{}

void AllPeaks::invalidateAll() const
{
    direct      .invalidate();
    interpolated.invalidate();
}

void AllPeaks::invalidateAt(int jP) const
{
    direct      .get(this,jP).invalidate();
    interpolated.get(this,jP).invalidate();
}

void AllPeaks::invalidateInterpolated() const
{
    interpolated.invalidate();
}

const PeakInfos* AllPeaks::currentDirect() const
{
    if (!gSession->peaks.count())
        return nullptr;
    ASSERT(direct.size(this)==gSession->peaks.count());
    int jP = gSession->peaks.selectedIndex();
    direct.getget(this,jP).inspect("currentDirect");
    return &direct.getget(this,jP);
}

const PeakInfos* AllPeaks::currentInterpolated() const
{
    if (!gSession->peaks.count())
        return nullptr;
    ASSERT(interpolated.size(this)==gSession->peaks.count());
    int jP = gSession->peaks.selectedIndex();
    return &interpolated.getget(this,jP);
}

const PeakInfos* AllPeaks::currentPeakInfos() const
{
    return gSession->params.interpolParams.enabled.val() ?
        currentInterpolated() : currentDirect();
}

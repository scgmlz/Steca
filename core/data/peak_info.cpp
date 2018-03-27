// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/peak_info.cpp
//! @brief     Implements classes PeakInfo, PeakInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

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
          Q_QNAN, Q_QNAN, Range(), float(Q_QNAN), float(Q_QNAN), deg(Q_QNAN), deg(Q_QNAN), Q_QNAN, Q_QNAN)
{}

PeakInfo::PeakInfo(shp_Metadata md, deg alpha, deg beta, Range rgeGma, float inten,
                   float intenError, deg tth, deg tthError, float fwhm, float fwhmError)
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

PeakInfo::PeakInfo(shp_Metadata md, deg alpha, deg beta, Range rgeGma)
    : PeakInfo(md, alpha, beta, rgeGma, float(Q_QNAN), float(Q_QNAN),
               deg(Q_QNAN), deg(Q_QNAN), float(Q_QNAN), float(Q_QNAN))
{}

PeakInfo::PeakInfo(deg alpha, deg beta, Range rgeGma, float inten, float intenError, deg tth,
                   deg tthError, float fwhm, float fwhmError)
    : PeakInfo(shp_Metadata(), alpha, beta, rgeGma, inten, intenError,
               tth, tthError, fwhm, fwhmError)
{}

PeakInfo::PeakInfo(deg alpha, deg beta)
    : PeakInfo(alpha, beta, Range(), float(Q_QNAN), float(Q_QNAN),
               deg(Q_QNAN), deg(Q_QNAN), float(Q_QNAN), float(Q_QNAN))
{}

QStringList PeakInfo::dataTags(bool out)
{
    QStringList tags;
    for_i (int(eReflAttr::NUM_REFL_ATTR))
        tags.append(reflStringTag(i, out));
    tags.append(Metadata::attributeTags(out));
    return tags;
}

QVector<VariantComparator*> PeakInfo::dataCmps()
{
    static QVector<VariantComparator*> cmps;
    if (cmps.isEmpty()) {
        cmps = QVector<VariantComparator*>{ cmp_real, cmp_real, cmp_real, cmp_real, cmp_real,
                        cmp_real, cmp_real, cmp_real, cmp_real, cmp_real };
        cmps.append(Metadata::attributeCmps());
    }
    return cmps;
}

QVector<QVariant> PeakInfo::data() const
{
    QVector<QVariant> row{ QVariant(alpha()),      QVariant(beta()),     QVariant(rgeGma().min),
                    QVariant(rgeGma().max), QVariant(inten()),    QVariant(intenError()),
                    QVariant(tth()),        QVariant(tthError()), QVariant(fwhm()),
                    QVariant(fwhmError()) };
    row.append(md_ ? md_->attributeValues() : Metadata::attributeNaNs());
    return row;
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


// ************************************************************************** //
//  class PeakInfos
// ************************************************************************** //

void PeakInfos::append(const PeakInfo& info)
{
    QVector<PeakInfo>::append(info);
    invalidate();
}

float PeakInfos::averageInten() const
{
    if (qIsNaN(avgInten_)) {
        avgInten_ = 0;
        int cnt = 0;
        for (auto& info : *this) {
            double inten = info.inten();
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

const Range& PeakInfos::rgeInten() const
{
    if (!rgeInten_.isValid()) {
        for_i (count())
            rgeInten_.extendBy(at(i).inten());
    }
    return rgeInten_;
}

void PeakInfos::invalidate()
{
    avgInten_ = float(Q_QNAN);
    rgeInten_.invalidate();
}

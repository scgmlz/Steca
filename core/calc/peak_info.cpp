//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/peak_info.cpp
//! @brief     Implements class PeakInfo
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/calc/peak_info.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

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

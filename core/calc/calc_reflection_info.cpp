// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/calc_reflection_info.cpp
//! @brief     Implements classes ReflectionInfo, ReflectionInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/calc/calc_reflection_info.h"
#include "core/data/metadata.h"
#include "core/def/idiomatic_for.h"

// ************************************************************************** //
//  class ReflectionInfo
// ************************************************************************** //

/* NOTE Invalid output parameters are set to NaNs. However, some analysis
 * programs
 * debug::ensure -1 as unknown value; thus, NaN parameter values should be output
 * as -1 when output is written for these programs (polefigure!).
 */

ReflectionInfo::ReflectionInfo()
    : ReflectionInfo(
          QSharedPointer<Metadata const>(),
          NAN, NAN, Range(), inten_t(NAN), inten_t(NAN), deg(NAN), deg(NAN), NAN, NAN)
{}

ReflectionInfo::ReflectionInfo(
    QSharedPointer<Metadata const> md,
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

ReflectionInfo::ReflectionInfo(
    QSharedPointer<Metadata const> md, deg alpha, deg beta, Range rgeGma)
    : ReflectionInfo(
        md, alpha, beta, rgeGma, inten_t(NAN), inten_t(NAN), deg(NAN), deg(NAN), fwhm_t(NAN),
        fwhm_t(NAN))
{}

ReflectionInfo::ReflectionInfo(
    deg alpha, deg beta, Range rgeGma, inten_t inten, inten_t intenError, deg tth,
    deg tthError, fwhm_t fwhm, fwhm_t fwhmError)
    : ReflectionInfo(
        QSharedPointer<Metadata const>(),
        alpha, beta, rgeGma, inten, intenError, tth, tthError, fwhm, fwhmError)
{}

ReflectionInfo::ReflectionInfo(deg alpha, deg beta)
    : ReflectionInfo(
          alpha, beta, Range(), inten_t(NAN), inten_t(NAN), deg(NAN), deg(NAN), fwhm_t(NAN),
          fwhm_t(NAN))
{}

QStringList ReflectionInfo::dataTags(bool out) {
    QStringList tags;
    for_i (uint(eReflAttr::NUM_REFL_ATTR))
        tags.append(reflStringTag(i, out));
    tags.append(Metadata::attributeTags(out));
    return tags;
}

cmp_vec ReflectionInfo::dataCmps() {
    static cmp_vec cmps;
    if (cmps.isEmpty()) {
        cmps = cmp_vec{ cmp_real, cmp_real, cmp_real, cmp_real, cmp_real,
                        cmp_real, cmp_real, cmp_real, cmp_real, cmp_real };
        cmps.append(Metadata::attributeCmps());
    }
    return cmps;
}

row_t ReflectionInfo::data() const {
    row_t row{ QVariant(alpha()),      QVariant(beta()),     QVariant(rgeGma().min),
                    QVariant(rgeGma().max), QVariant(inten()),    QVariant(intenError()),
                    QVariant(tth()),        QVariant(tthError()), QVariant(fwhm()),
                    QVariant(fwhmError()) };

    row.append(md_ ? md_->attributeValues() : Metadata::attributeNaNs());
    return row;
}

str const ReflectionInfo::reflStringTag(uint attr, bool out) {
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
    default: NEVER; return "";
    }
}


// ************************************************************************** //
//  class ReflectionInfos
// ************************************************************************** //

void ReflectionInfos::append(ReflectionInfo const& info) {
    super::append(info);
    invalidate();
}

inten_t ReflectionInfos::averageInten() const {
    if (qIsNaN(avgInten_)) {
        avgInten_ = 0;
        uint cnt = 0;
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

Range const& ReflectionInfos::rgeInten() const {
    if (!rgeInten_.isValid()) {
        for_i (count())
            rgeInten_.extendBy(at(i).inten());
    }
    return rgeInten_;
}

void ReflectionInfos::invalidate() {
    avgInten_ = inten_t(NAN);
    rgeInten_.invalidate();
}

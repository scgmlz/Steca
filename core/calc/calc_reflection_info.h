// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_reflection_info.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CALC_REFLECTION_INFO_H
#define CALC_REFLECTION_INFO_H

#include "data/data_dataset.h"
#include "def/defs.h"
#include "typ/typ_strlst.h"
#include "typ/typ_types.h"
#include "typ/typ_variant.h"

namespace calc {

class ReflectionInfo final {
    CLASS(ReflectionInfo)
public:
    enum class eField {
        ALPHA,
        BETA,
        GAMMA1,
        GAMMA2,
        INTEN,
        INTEN_ERROR,
        TTH,
        INTEN_TTH,
        FWHM,
        INTEN_FWHM,
    };

    enum class eReflAttr {
        ALPHA,
        BETA,
        GAMMA1,
        GAMMA2,
        INTEN,
        SIGMA_INTEN,
        TTH,
        SIGMA_TTH,
        FWHM,
        SIGMA_FWHM,
        NUM_REFL_ATTR,
    };

    static str_lst dataTags(bool out);
    static typ::cmp_vec dataCmps();

    static str const reflStringTag(uint attr, bool out);

    ReflectionInfo();

    ReflectionInfo(
        data::shp_Metadata, typ::deg alpha, typ::deg beta, gma_rge, inten_t, inten_t /*error*/,
        tth_t, tth_t /*error*/, fwhm_t, fwhm_t /*error*/);
    ReflectionInfo(data::shp_Metadata, typ::deg alpha, typ::deg beta, gma_rge);
    ReflectionInfo(
        typ::deg alpha, typ::deg beta, gma_rge, inten_t, inten_t /*error*/, tth_t, tth_t /*error*/,
        fwhm_t, fwhm_t /*error*/);
    ReflectionInfo(typ::deg alpha, typ::deg beta);

    typ::deg alpha() const { return alpha_; }
    typ::deg beta() const { return beta_; }

    gma_rge rgeGma() const { return rgeGma_; }

    inten_t inten() const { return inten_; }
    inten_t intenError() const { return intenError_; }

    tth_t tth() const { return tth_; }
    tth_t tthError() const { return tthError_; }

    fwhm_t fwhm() const { return fwhm_; }
    fwhm_t fwhmError() const { return fwhmError_; }

    typ::row_t data() const;

private:
    data::shp_Metadata md_;

    typ::deg alpha_, beta_;
    gma_rge rgeGma_;

    inten_t inten_, intenError_;
    tth_t tth_, tthError_;
    fwhm_t fwhm_, fwhmError_;
};

class ReflectionInfos : public typ::vec<ReflectionInfo> {
    CLASS(ReflectionInfos) SUPER(typ::vec<ReflectionInfo>);
public:
    ReflectionInfos();

    void append(ReflectionInfo::rc);

    inten_t averageInten() const;
    inten_rge::rc rgeInten() const;

private:
    void invalidate();

    mutable inten_t avgInten_;
    mutable inten_rge rgeInten_;
};
}
#endif // CALC_REFLECTION_INFO_H

// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_reflection_info.h
//! @brief     Defines classes ReflectionInfo, ReflectionInfos
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CALC_REFLECTION_INFO_H
#define CALC_REFLECTION_INFO_H

#include "data/dataset.h"
#include "typ/angles.h"
#include "typ/range.h"
#include "typ/vec.h"

using typ::deg;

namespace calc {

class ReflectionInfo final {
    public:
    ReflectionInfo();
    ReflectionInfo(
        data::shp_Metadata, typ::deg alpha, typ::deg beta, typ::Range, inten_t, inten_t /*error*/,
        deg, deg /*error*/, fwhm_t, fwhm_t /*error*/);
    ReflectionInfo(data::shp_Metadata, typ::deg alpha, typ::deg beta, typ::Range);
    ReflectionInfo(
        typ::deg alpha, typ::deg beta, typ::Range, inten_t, inten_t /*error*/, deg, deg /*error*/,
        fwhm_t, fwhm_t /*error*/);
    ReflectionInfo(typ::deg alpha, typ::deg beta);

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

    typ::deg alpha() const { return alpha_; }
    typ::deg beta() const { return beta_; }
    typ::Range rgeGma() const { return rgeGma_; }
    inten_t inten() const { return inten_; }
    inten_t intenError() const { return intenError_; }
    deg tth() const { return tth_; }
    deg tthError() const { return tthError_; }
    fwhm_t fwhm() const { return fwhm_; }
    fwhm_t fwhmError() const { return fwhmError_; }
    typ::row_t data() const;

private:
    data::shp_Metadata md_;
    typ::deg alpha_, beta_;
    typ::Range rgeGma_;
    inten_t inten_, intenError_;
    deg tth_, tthError_;
    fwhm_t fwhm_, fwhmError_;

    static str const reflStringTag(uint attr, bool out);
};


class ReflectionInfos : public typ::vec<ReflectionInfo> {
private:
    using super = typ::vec<ReflectionInfo>;
public:
    ReflectionInfos() { invalidate(); }

    void append(ReflectionInfo const&);

    inten_t averageInten() const;
    typ::Range const& rgeInten() const;

private:
    mutable inten_t avgInten_;
    mutable typ::Range rgeInten_;

    void invalidate();
};

} // namespace calc

#endif // CALC_REFLECTION_INFO_H

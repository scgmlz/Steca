// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/reflection_info.h
//! @brief     Defines classes ReflectionInfo, ReflectionInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef REFLECTION_INFO_H
#define REFLECTION_INFO_H

#include "core/data/metadata.h"
#include "core/typ/angles.h"
#include "core/typ/range.h"
#include "core/typ/types.h"
#include "core/typ/variant.h"

//! Metadata, peak fit results, and pole figure angles.

class ReflectionInfo final {
    public:
    ReflectionInfo();
    ReflectionInfo(
        shp_Metadata,
        deg alpha, deg beta, Range, inten_t, inten_t /*error*/,
        deg, deg /*error*/, fwhm_t, fwhm_t /*error*/);
    ReflectionInfo(shp_Metadata, deg alpha, deg beta, Range);
    ReflectionInfo(
        deg alpha, deg beta, Range, inten_t, inten_t /*error*/, deg, deg /*error*/,
        fwhm_t, fwhm_t /*error*/);
    ReflectionInfo(deg alpha, deg beta);

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

    static QStringList dataTags(bool out);
    static cmp_vec dataCmps();

    deg alpha() const { return alpha_; }
    deg beta() const { return beta_; }
    Range rgeGma() const { return rgeGma_; }
    inten_t inten() const { return inten_; }
    inten_t intenError() const { return intenError_; }
    deg tth() const { return tth_; }
    deg tthError() const { return tthError_; }
    fwhm_t fwhm() const { return fwhm_; }
    fwhm_t fwhmError() const { return fwhmError_; }
    row_t data() const;

private:
    shp_Metadata md_;
    deg alpha_, beta_;
    Range rgeGma_;
    inten_t inten_, intenError_;
    deg tth_, tthError_;
    fwhm_t fwhm_, fwhmError_;

    static str const reflStringTag(uint attr, bool out);
};


//! A list of ReflectionInfo's

class ReflectionInfos : public vec<ReflectionInfo> {
public:
    ReflectionInfos() { invalidate(); }

    void append(ReflectionInfo const&);

    inten_t averageInten() const;
    const Range& rgeInten() const;

private:
    mutable inten_t avgInten_;
    mutable Range rgeInten_;

    void invalidate();
};

#endif // REFLECTION_INFO_H

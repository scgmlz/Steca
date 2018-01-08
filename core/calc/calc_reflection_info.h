// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/calc_reflection_info.h
//! @brief     Defines classes ReflectionInfo, ReflectionInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CALC_REFLECTION_INFO_H
#define CALC_REFLECTION_INFO_H

#include "core/typ/angles.h"
#include "core/typ/range.h"
#include "core/typ/types.h"
#include "core/typ/variant.h"
#include <QSharedPointer> // no auto rm

class Metadata;

class ReflectionInfo final {
    public:
    ReflectionInfo();
    ReflectionInfo(
        QSharedPointer<Metadata const>,
        deg alpha, deg beta, Range, inten_t, inten_t /*error*/,
        deg, deg /*error*/, fwhm_t, fwhm_t /*error*/);
    ReflectionInfo(QSharedPointer<Metadata const>, deg alpha, deg beta, Range);
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
    QSharedPointer<Metadata const> md_;
    deg alpha_, beta_;
    Range rgeGma_;
    inten_t inten_, intenError_;
    deg tth_, tthError_;
    fwhm_t fwhm_, fwhmError_;

    static str const reflStringTag(uint attr, bool out);
};


class ReflectionInfos : public vec<ReflectionInfo> {
private:
    using super = vec<ReflectionInfo>;
public:
    ReflectionInfos() { invalidate(); }

    void append(ReflectionInfo const&);

    inten_t averageInten() const;
    Range const& rgeInten() const;

private:
    mutable inten_t avgInten_;
    mutable Range rgeInten_;

    void invalidate();
};

#endif // CALC_REFLECTION_INFO_H

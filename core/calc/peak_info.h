// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/peak_info.h
//! @brief     Defines classes PeakInfo, PeakInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PEAK_INFO_H
#define PEAK_INFO_H

#include "core/data/metadata.h"
#include "core/typ/range.h"
#include "core/typ/types.h"

//! Metadata, peak fit results, and pole figure angles.

class PeakInfo final {
public:
    PeakInfo();
    PeakInfo(
        shp_Metadata,
        deg alpha, deg beta, Range, inten_t, inten_t /*error*/,
        deg, deg /*error*/, fwhm_t, fwhm_t /*error*/);
    PeakInfo(shp_Metadata, deg alpha, deg beta, Range);
    PeakInfo(
        deg alpha, deg beta, Range, inten_t, inten_t /*error*/, deg, deg /*error*/,
        fwhm_t, fwhm_t /*error*/);
    PeakInfo(deg alpha, deg beta);
    // TODO revert from constructor to function to emphasize that all the fitting takes place here
    PeakInfo(const class Cluster* cluster, const class Peak& peak, const Range& gmaSector);

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

    static QString const reflStringTag(int attr, bool out);
};


//! A list of PeakInfo's

class PeakInfos : public QVector<PeakInfo> {
public:
    PeakInfos() { invalidate(); }
    // TODO revert from constructor to function to emphasize that all the fitting takes place here
    PeakInfos(const class Peak& peak, class Progress* progress);

    void append(const PeakInfo&);

    inten_t averageInten() const;
    const Range& rgeInten() const;

private:
    mutable inten_t avgInten_;
    mutable Range rgeInten_;

    void invalidate();
};

#endif // PEAK_INFO_H

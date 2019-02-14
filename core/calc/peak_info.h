//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/peak_info.h
//! @brief     Defines class PeakInfo
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PEAK_INFO_H
#define PEAK_INFO_H

#include "core/raw/metadata.h"
#include "core/typ/range.h"

//! Metadata, peak fit results, and pole figure angles.

class PeakInfo final {
public:
    PeakInfo(const Metadata*, deg alpha, deg beta, Range, double, double /*error*/,
             deg, deg /*error*/, double, double /*error*/, double, double /*error*/);
    PeakInfo(const Metadata*, deg alpha, deg beta, Range, double, double /*error*/,
             deg, deg /*error*/, double, double /*error*/);
    PeakInfo(const Metadata*, deg alpha, deg beta, Range);
    PeakInfo(deg alpha, deg beta, Range, double, double /*error*/, deg, deg /*error*/,
             double, double /*error*/);
    PeakInfo(deg alpha, deg beta);
    PeakInfo(const PeakInfo&) = delete;
    PeakInfo(PeakInfo&&) = default;

    static QStringList dataTags(bool out);
    static QStringList metaTags();
    static std::vector<VariantComparator*> dataCmps();

    deg alpha() const { return alpha_; }
    deg beta() const { return beta_; }
    Range rgeGma() const { return rgeGma_; }
    double inten() const { return inten_; }
    double intenError() const { return intenError_; }
    deg tth() const { return tth_; }
    deg tthError() const { return tthError_; }
    double fwhm() const { return fwhm_; }
    double fwhmError() const { return fwhmError_; }
    double gammOverSigma() const { return gammOverSigma_; }
    double gammOverSigmaError() const { return gammOverSigmaError_; }
    std::vector<QVariant> peakData() const;

    static bool hasSigma(int index);

private:
    const Metadata* md_;
    deg alpha_, beta_;
    Range rgeGma_;
    double inten_, intenError_;
    deg tth_, tthError_;
    double fwhm_, fwhmError_;
    double gammOverSigma_, gammOverSigmaError_;
};

#endif // PEAK_INFO_H

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
#include "core/pars/onepeak_settings.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class PeakInfo
//!
//! Invalid output parameters are set to NaNs. However, some analysis programs
//! debug::ensure -1 as unknown value; thus, NaN parameter values should be output
//! as -1 when output is written for these programs (polefigure!).

PeakInfo::PeakInfo(const Metadata* md,
                   deg alpha, deg beta, Range rgeGma, double inten,
                   double intenError, deg tth, deg tthError, double fwhm, double fwhmError,
                   double sog, double sogError)
    : md_{md}
    , alpha_ {alpha}
    , beta_ {beta}
    , rgeGma_ {rgeGma}
    , inten_ {inten}
    , intenError_ {intenError}
    , tth_ {tth}
    , tthError_ {tthError}
    , fwhm_ {fwhm}
    , fwhmError_ {fwhmError}
    , gammOverSigma_ {sog}
    , gammOverSigmaError_ {sogError}
{}

PeakInfo::PeakInfo(const Metadata* md,
                    deg alpha, deg beta, Range rgeGma)
    : PeakInfo {md, alpha, beta, rgeGma,
                Q_QNAN, Q_QNAN, deg(Q_QNAN), deg(Q_QNAN), Q_QNAN, Q_QNAN, Q_QNAN, Q_QNAN}
{}

PeakInfo::PeakInfo(deg alpha, deg beta, Range rgeGma, double inten, double intenError, deg tth,
                   deg tthError, double fwhm, double fwhmError)
    : PeakInfo{nullptr, alpha, beta, rgeGma,
                inten, intenError, tth, tthError, fwhm, fwhmError, Q_QNAN, Q_QNAN}
{}

PeakInfo::PeakInfo(deg alpha, deg beta)
    : PeakInfo {nullptr, alpha, beta, Range(),
                Q_QNAN, Q_QNAN, deg(Q_QNAN), deg(Q_QNAN), Q_QNAN, Q_QNAN, Q_QNAN, Q_QNAN}
{}

std::vector<QVariant> PeakInfo::peakData() const
{
    std::vector<QVariant> ret{
        QVariant(alpha_),
        QVariant(beta_),
        QVariant(rgeGma_.min),
        QVariant(rgeGma_.max),
        QVariant(inten_),
        QVariant(intenError_),
        QVariant(tth_),
        QVariant(tthError_),
        QVariant(fwhm_),
        QVariant(fwhmError_) };
    if (gSession->peaksSettings.selectedPeak()->fitParAsciiNames().contains("Gamma/Sigma")) {
        ret.push_back( QVariant(gammOverSigma_) );
        ret.push_back( QVariant(gammOverSigmaError_) );
    }
    auto values_to_append = md_ ? md_->attributeValues() : Metadata::attributeNaNs();
    ret.insert(ret.end(), values_to_append.begin(), values_to_append.end());
    return ret;
}

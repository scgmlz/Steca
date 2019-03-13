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

PeakInfo::PeakInfo(const Metadata* md, deg alpha, deg beta, Range rgeGma,
                   const PeakOutcome& outcome)
    : md_{md}
    , alpha_{alpha}
    , beta_{beta}
    , rgeGma_{rgeGma}
    , outcome_{outcome}
{}

PeakInfo::PeakInfo(deg alpha, deg beta)
    : PeakInfo{nullptr, alpha, beta, Range(), {}}
{}

std::vector<QVariant> PeakInfo::peakData() const
{
    std::vector<QVariant> ret{
        QVariant(alpha_),
        QVariant(beta_),
        QVariant(rgeGma_.min),
        QVariant(rgeGma_.max)};
    for (const QString& key: {"intensity", "center", "fwhm", "gammaOverSigma"}) {
        if (outcome_.has(key)) {
            ret.push_back( QVariant(outcome_.at(key)) );
            ret.push_back( QVariant(outcome_.at("sigma_"+key)) );
        }
    }
    auto values_to_append = md_ ? md_->attributeValues() : Metadata::attributeNaNs();
    ret.insert(ret.end(), values_to_append.begin(), values_to_append.end());
    return ret;
}

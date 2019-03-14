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

#include "core/base/angles.h"
#include "core/typ/range.h"
#include "core/typ/mapped.h"

class Metadata;
class OnePeakSettings;

//! Metadata, peak fit results, and pole figure angles.

class PeakInfo final {
public:
    PeakInfo(const Metadata*, deg alpha, deg beta, Range range, const Mapped& outcome={});
    PeakInfo(deg alpha, deg beta);
    PeakInfo(const PeakInfo&) = delete;
    PeakInfo(PeakInfo&&) = default;

    deg alpha() const { return alpha_; }
    deg beta() const { return beta_; }
    Range rgeGma() const { return rgeGma_; }
    const Mapped& outcome() const { return outcome_; }
    std::vector<QVariant> peakData() const;

private:
    const Metadata*const md_;
    deg alpha_, beta_;
    Range rgeGma_;
    const Mapped outcome_;
};

#endif // PEAK_INFO_H

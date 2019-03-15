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

#include "core/typ/mapped.h"

class Metadata;

//! Metadata, peak fit results, and pole figure angles.

class PeakInfo final {
public:
    PeakInfo(const Metadata*, const Mapped& outcome={});
    PeakInfo(const PeakInfo&) = delete;
    PeakInfo(PeakInfo&&) = default;

    const Mapped& map() const { return outcome_; }
    std::vector<QVariant> peakData() const;
    double at(const QString& key) const { return outcome_.at(key); }
    bool has(const QString& key) const { return outcome_.has(key); }

private:
    const Metadata*const md_;
    const Mapped outcome_;
};

#endif // PEAK_INFO_H

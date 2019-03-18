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

class PeakInfo final : public Mapped {
public:
    PeakInfo(const Metadata* md, const Mapped& outcome)
        : Mapped{outcome}, md_{md} {}

    const Mapped& map() const { return *dynamic_cast<const Mapped*>(this); }
    const Metadata*const md_;
};


#endif // PEAK_INFO_H

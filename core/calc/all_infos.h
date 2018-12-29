//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/all_infos.h
//! @brief     Defines class AllInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef ALL_INFOS_H
#define ALL_INFOS_H

#include "core/calc/info_sequence.h"
#include "lazy_data.h"

//! Direct and interpolated InfoSequence for all Bragg peaks.

class AllInfos {
public:
    AllInfos();
    AllInfos(const AllInfos&) = delete;
    const InfoSequence* currentDirect() const;
    const InfoSequence* currentInterpolated() const;
    const InfoSequence* currentInfoSequence() const;
    const std::vector<InfoSequence const *> allDirect() const;
    const std::vector<InfoSequence const *> allInterpolated() const;
    const std::vector<InfoSequence const *> allInfoSequences() const;
    void invalidateAll() const;
    void invalidateInterpolated() const;
    void invalidateAt(int) const;
private:
    mutable lazy_data::VectorCache<InfoSequence,AllInfos> direct;
    mutable lazy_data::VectorCache<InfoSequence,AllInfos> interpolated;
};

#endif // ALL_INFOS_H

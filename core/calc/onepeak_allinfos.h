//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/onepeak_allinfos.h
//! @brief     Defines class InfoSequence
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef ONEPEAK_ALLINFOS_H
#define ONEPEAK_ALLINFOS_H

#include "core/calc/peak_info.h"

//! A list of `PeakInfo`s, associated with _one_ Bragg peak and different orientations alpha,beta.

class OnePeakAllInfos {
public:
    OnePeakAllInfos() {}
    OnePeakAllInfos(const OnePeakAllInfos&) = delete;
    OnePeakAllInfos(OnePeakAllInfos&&) = default;

    void appendPeak(PeakInfo&&);

    const std::vector<PeakInfo>& peakInfos() const { return peakInfos_; }
    void get4(const int idxX, const int idxY,
              std::vector<double>& xs, std::vector<double>& ys,
              std::vector<double>& ysLow, std::vector<double>& ysHig) const;
    void getValuesAndSigma(const size_t idxX, const size_t idxY,
              std::vector<double>& xs, std::vector<double>& ys,
              std::vector<double>& ysSigma) const;
private:
    std::vector<PeakInfo> peakInfos_;
};

#endif // ONEPEAK_ALLINFOS_H

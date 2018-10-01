//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/info_sequence.h
//! @brief     Defines class InfoSequence
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef INFO_SEQUENCE_H
#define INFO_SEQUENCE_H

#include "core/calc/peak_info.h"

//! A list of PeakInfo's, associated with _one_ Bragg peak and different orientations alpha,beta.

class InfoSequence {
public:
    InfoSequence() {}
    InfoSequence(const InfoSequence&) = delete;
    InfoSequence(InfoSequence&&) = default;

    void appendPeak(PeakInfo&&);

    const std::vector<PeakInfo>& peaks() const { return peaks_; }
    void get4(const int idxX, const int idxY,
              std::vector<double>& xs, std::vector<double>& ys,
              std::vector<double>& ysLow, std::vector<double>& ysHig) const;
    void getValuesAndSigma(const size_t idxX, const size_t idxY,
              std::vector<double>& xs, std::vector<double>& ys,
              std::vector<double>& ysSigma) const;
    void inspect(const QString& header) const;

private:
    std::vector<PeakInfo> peaks_;
};

#endif // INFO_SEQUENCE_H

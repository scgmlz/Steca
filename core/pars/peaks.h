//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/peaks.h
//! @brief     Defines class Peaks
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PEAKS_H
#define PEAKS_H

#include "core/pars/peak_fitpar.h"

//! All user defined peaks, of which one is selected to be acted on by default.

class Peaks {
public:
    void clear();
    void fromJson(const QJsonArray& arr);
    void add(const Range&);
    void removeSelected();
    void select(int i) { selected_ = i; }
    bool selectByValue(double x);

    PeakFitpar* selectedPeak() {
        return 0<=selected_ && selected_<size() ? &peaks_[selected_] : nullptr; }

    int size() const { return peaks_.size(); }
    const PeakFitpar& at(int i) const { return peaks_.at(i); }
    PeakFitpar& at(int i) { return peaks_.at(i); }
    int selectedIndex() { return selected_; }
    QJsonArray toJson() const;

private:
    void sort();
    void doAdd(PeakFitpar&& peak);
    int selected_ {-1};
    std::vector<PeakFitpar> peaks_;
};

#endif // PEAKS_H

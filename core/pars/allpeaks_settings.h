//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/allpeaks_settings.h
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

#include "core/pars/onepeak_settings.h"

//! All user defined peaks, of which one is selected to be acted on by default.

class AllPeaksSettings {
public:
    void clear();
    void fromJson(const QJsonArray& arr);
    void add(const Range&);
    void removeSelected();
    void select(int i) { selected_ = i; }
    bool selectByValue(double x);

    OnePeakSettings* selectedPeak() {
        return 0<=selected_ && selected_<size() ? &peaksSettings_[selected_] : nullptr; }

    int size() const { return peaksSettings_.size(); }
    const OnePeakSettings& at(int i) const { return peaksSettings_.at(i); }
    OnePeakSettings& at(int i) { return peaksSettings_.at(i); }
    int selectedIndex() { return selected_; }
    QJsonArray toJson() const;

private:
    void sort();
    void doAdd(OnePeakSettings&& peak);
    int selected_ {-1};
    std::vector<OnePeakSettings> peaksSettings_;
};

#endif // PEAKS_H

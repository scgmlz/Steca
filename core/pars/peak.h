//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/peak.h
//! @brief     Defines classes Peak and Peaks
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PEAK_H
#define PEAK_H

#include "core/fit/fit_fun.h"
#include <memory>

//! Wraps a PeakFunction (pimpl idiom)

class Peak {
public:
    Peak() = delete;
    Peak(const Range& range, const QString& functionName = "Raw");

    static Peak fromJson(const JsonObj&);

    void setPeakFunction(const QString&);
    void setRange(const Range&);
    void invalidateGuesses();
    void setGuessPeak(const qpair& peak);
    void setGuessFWHM(float fwhm);
    void fit(const Curve&);

    Range& range() { return peakFunction_->fitRange(); }
    const Range& range() const { return peakFunction_->fitRange(); }

    const PeakFunction& peakFunction() const;
    QString functionName() const { return peakFunction_->name(); }
    bool isRaw() const { return peakFunction_->isRaw(); }
    JsonObj toJson() const;

private:
    std::unique_ptr<PeakFunction> peakFunction_; //!< pimpl (pointer to implementation)
};


//! All user defined peaks, of which one is selected to be acted on by default.

class Peaks {
public:
    static QString defaultFunctionName;

    void clear();
    void fromJson(const QJsonArray& arr);
    void add(const Range&);
    void removeSelected();
    void select(int i) { selected_ = i; }
    void selectByValue(double x);

    Peak* selectedPeak() {
        return 0<=selected_ && selected_<count() ? &peaks_[selected_] : nullptr; }
    Range* selectedRange() {
        return selectedPeak() ? &selectedPeak()->range() : nullptr; }

    int count() const { return peaks_.size(); }
    const Peak& at(int i) const { return peaks_.at(i); }
    Peak& at(int i) { return peaks_.at(i); }
    int selectedIndex() { return selected_; }
    QJsonArray toJson() const;

private:
    void sort();
    void doAdd(Peak&& peak);
    int selected_ {-1};
    std::vector<Peak> peaks_;
};

#endif // PEAK_H

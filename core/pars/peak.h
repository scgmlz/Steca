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

#include "core/typ/range.h"

//! Global settings (range, function type) for one Bragg peak.

class Peak {
public:
    Peak(const Range& range, const QString& functionName);

    void setPeakFunction(const QString&);
    void setRange(const Range&);
    void setOne(double val, bool namelyMax); //!< sets either min or max

    const Range& range() const { return range_; }

    QString functionName() const { return functionName_; }
    bool isRaw() const { return functionName_=="Raw"; }
    JsonObj toJson() const;

    static Peak fromJson(const JsonObj&);
    static const QStringList keys;

private:
    Range range_;
    QString functionName_;
};


//! All user defined peaks, of which one is selected to be acted on by default.

class Peaks {
public:
    void clear();
    void fromJson(const QJsonArray& arr);
    void add(const Range&);
    void removeSelected();
    void select(int i) { selected_ = i; }
    bool selectByValue(double x);

    Peak* selectedPeak() {
        return 0<=selected_ && selected_<size() ? &peaks_[selected_] : nullptr; }

    int size() const { return peaks_.size(); }
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

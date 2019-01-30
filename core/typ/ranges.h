//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/ranges.h
//! @brief     Defines class Ranges
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef RANGES_H
#define RANGES_H

#include "core/typ/range.h"
#include <QJsonArray>
#include <vector> // no auto rm

class JsonObj;

//! A *sorted* list of *non-overlapping* ranges, of which one is always selected.
class Ranges {
public:
    void fromJson(const QJsonArray&);
    void clear();
    void add(const Range&);
    void removeSelected();
    void select(size_t i);
    bool selectByValue(double x);

    Range* selectedRange() { return isEmpty() ? nullptr : &ranges_.at(selected_); }

    QJsonArray toJson() const;
    bool isEmpty() const { return ranges_.empty(); }
    size_t size() const { return ranges_.size(); }
    const Range& at(size_t i) const { return ranges_[i]; }
    size_t selectedIndex() const { return selected_; }

private:
    void sort();
    std::vector<Range> ranges_;
    size_t selected_;
};

#endif // RANGES_H

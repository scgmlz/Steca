//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/ranges.cpp
//! @brief     Implements class Ranges
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/typ/ranges.h"
#include "core/base/exception.h"
#include "core/typ/json.h"
#include "qcr/base/debug.h"


void Ranges::clear() {
    ranges_.clear();
}

//! Adds given range to *this, and removes ranges from *this that intersect.
void Ranges::add(const Range& range)
{
    ranges_.erase(std::remove_if(ranges_.begin(), ranges_.end(), [=](const Range& r) {
                return r.intersects(range); }), ranges_.end());
    ranges_.push_back(range);
    sort();
    // not elegant: find the newly added range
    for (size_t i=0; i<size(); ++i) {
        if (at(i).intersects(range)) {
            selected_ = i;
            return;
        }
    }
}

void Ranges::removeSelected()
{
    ASSERT(0<=selected_ && selected_<size());
    ranges_.erase(ranges_.begin()+selected_);
    selected_ -= 1;
    if (selected_<0 && size())
        selected_ = 0;
}

void Ranges::select(size_t i)
{
    ASSERT(0<=i && i<size());
    selected_ = i;
}

//! Selects the range that contains x. If there is no such range, then selected_ is left unchanged.
//! Returns true if a range has been found else returns false.
bool Ranges::selectByValue(double x)
{
    for (size_t i=0; i<size(); ++i) {
        if (at(i).contains(x)) {
            selected_ = i;
            return true;
        }
    }
    return false;
}

void Ranges::sort()
{
    std::sort(ranges_.begin(), ranges_.end(), [](const Range& r1, const Range& r2) {
            return r1.min < r2.min; } );
}

QJsonArray Ranges::toJson() const
{
    QJsonArray arr;
    for (const Range& range : ranges_)
        arr.append(range.toJson());
    return arr;
}

void Ranges::fromJson(const QJsonArray& arr)
{
    for (const auto& ele: arr) {
        Range range;
        range.fromJson(ele.toObject());
        ranges_.push_back(range);
    }
}

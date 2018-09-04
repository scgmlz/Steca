//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/range.cpp
//! @brief     Implements classes Range and Ranges
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test006_range (good coverage since this is somewhat involved)
//
//  ***********************************************************************************************

#include "core/typ/range.h"
#include "core/aux/exception.h"
#include "core/typ/json.h"
#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class Range

Range::Range()
{
    invalidate();
}

Range::Range(double min, double max)
{
    set(min, max);
}

Range::Range(const std::vector<double>& vec)
{
    if (!vec.size()) {
        invalidate();
        return;
    }
    double min = vec[0];
    double max = vec[0];
    for (int i=1; i<vec.size(); ++i) {
        if (vec[i]<min) min = vec[i];
        if (vec[i]>max) max = vec[i];
    }
    set(min, max);
}

Range Range::infinite()
{
    return Range(-Q_INFINITY, +Q_INFINITY);
}

void Range::invalidate()
{
    set(Q_QNAN, Q_QNAN);
}

bool Range::isValid() const
{
    return !qIsNaN(min) && !qIsNaN(max);
}

bool Range::isEmpty() const
{
    return !isValid() || min >= max;
}

double Range::width() const
{
    return isValid() ? max - min : Q_QNAN;
}

double Range::center() const
{
    return isValid() ? (min + max) / 2 : Q_QNAN;
}

Range Range::slice(int i, int n) const
{
    if (!isValid())
        THROW("BUG: Range::slice called for invalid range");
    if (n<1)
        THROW("BUG: Range::slice called with invalid n="+QString::number(n));
    if (i<0 || i>=n)
        THROW("BUG  Range::slice called with invalid i="
              +QString::number(i)+" where n="+QString::number(n));
    double delta = width()/n;
    return Range(min+i*delta, min+(i+1)*delta);
}

void Range::set(double _min, double _max)
{
    min = _min;
    max = _max;
    ASSERT(!isValid() || min <= max);
}

void Range::setOne(double val, bool namelyMax) // sets either min or max
{
    if (namelyMax)
        max = val;
    else
        min = val;
    ASSERT(!isValid() || min <= max);
}

Range Range::safeFrom(double v1, double v2)
{
    if (v1<=v2)
        return {v1, v2};
    return {v2, v1};
}

void Range::extendBy(double val)
{
    min = qIsNaN(min) ? val : qMin(min, val);
    max = qIsNaN(max) ? val : qMax(max, val);
}

void Range::extendBy(const Range& that)
{
    extendBy(that.min);
    extendBy(that.max);
}

bool Range::contains(double val) const
{
    return min <= val && val <= max;
}

bool Range::contains(const Range& that) const
{
    return min <= that.min && that.max <= max;
}

bool Range::intersects(const Range& that) const
{
    return min <= that.max && that.min <= max;
}

Range Range::intersect(const Range& that) const
{
    if (!isValid() || !that.isValid())
        return Range();

    const double min_ = qMax(min, that.min), max_ = qMin(max, that.max);
    if (min_ <= max_)
        return Range(min_, max_);

    // disjoint
    const double val = that.min < min ? min : max;
    return Range(val, val); // empty, isValid()
}

QJsonObject Range::toJson() const
{
    return { { "min", double_to_json(min) }, { "max", double_to_json(max) } };
}

void Range::fromJson(const JsonObj& obj)
{
    min = obj.loadQreal("min");
    max = obj.loadQreal("max");
}

QString Range::to_s(const QString& unit, int precision, int digitsAfter) const
{
    return isValid()
        ? QString("%1 .. %2 %3")
        .arg(min, precision, 'f', digitsAfter)
        .arg(max, precision, 'f', digitsAfter)
        .arg(unit)
        : "undefined";
}


//  ***********************************************************************************************
//! @class Ranges

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
    for (int i=0; i<size(); ++i) {
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

void Ranges::select(int i)
{
    ASSERT(0<=i && i<size());
    selected_ = i;
}

//! Selects the range that contains x. If there is no such range, then selected_ is left unchanged.
//! Returns true if a range has been found else returns false.
bool Ranges::selectByValue(double x)
{
    for (int i=0; i<size(); ++i) {
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

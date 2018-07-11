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

#include "range.h"
#include "core/def/comparators.h"
#include "core/def/idiomatic_for.h"
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

int Range::compare(const Range& that) const
{
    ASSERT(isValid() && that.isValid());
    RET_COMPARE_VALUE(min)
    RET_COMPARE_VALUE(max)
    return 0;
}

VALID_EQ_NE_OPERATOR(Range)

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

void Range::set(double min_, double max_) {
    min = min_;
    max = max_;
    ASSERT(!isValid() || min <= max);
}

void Range::safeSet(double v1, double v2)
{
    if (v1 > v2)
        qSwap(v1, v2);
    set(v1, v2);
}

Range Range::safeFrom(double v1, double v2)
{
    Range range;
    range.safeSet(v1, v2);
    return range;
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

double Range::bound(double value) const
{
    if (isValid() && !qIsNaN(value))
        return qBound(min, value, max);
    return Q_QNAN;
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

void Ranges::add(const Range& range)
{
    std::vector<Range> newRanges;
    Range addRange = range;
    for (const Range& r : ranges_) {
        if (r.contains(range))
            return;
        if (!range.contains(r)) {
            if (range.intersects(r))
                addRange.extendBy(r);
            else
                newRanges.push_back(r);
        }
    }
    newRanges.push_back(addRange);
    ranges_ = newRanges;
    sort();
    return;
}

bool Ranges::remove(const Range& removeRange)
{
    std::vector<Range> newRanges;
    bool changed = false;

    for (const Range& r : ranges_) {
        if (!r.intersect(removeRange).isEmpty()) {
            changed = true;
            if (r.min < removeRange.min)
                newRanges.push_back(Range(r.min, removeRange.min));
            if (r.max > removeRange.max)
                newRanges.push_back(Range(removeRange.max, r.max));
        } else {
            newRanges.push_back(r);
        }
    }

    if (changed)
        ranges_ = newRanges;
    return changed;
}

static bool lessThan(const Range& r1, const Range& r2)
{
    if (r1.min < r2.min)
        return true;
    if (r1.min > r2.min)
        return false;
    return r1.max < r2.max;
}

void Ranges::sort()
{
    std::sort(ranges_.begin(), ranges_.end(), lessThan);
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
    for_i (arr.count()) {
        Range range;
        range.fromJson(arr.at(i).toObject());
        ranges_.push_back(range);
    }
}

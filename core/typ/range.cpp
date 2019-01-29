//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/range.cpp
//! @brief     Implements class Range
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
#include "core/base/exception.h"
#include "core/typ/json.h"
#include "qcr/base/debug.h"

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
    invalidate(); //always invalidate, in case vec contains *only*  NaNs.
    for (auto val : vec)
        extendBy(val);
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
    min = qIsNaN(min) ? val : qIsNaN(val) ? min : qMin(min, val);
    max = qIsNaN(max) ? val : qIsNaN(val) ? max : qMax(max, val);
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

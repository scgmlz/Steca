// ************************************************************************** //
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
// ************************************************************************** //

#include "core/def/comparators.h"
#include "core/def/idiomatic_for.h"
#include "core/typ/json.h"
#include "core/typ/range.h"

// ************************************************************************** //
//  class Range
// ************************************************************************** //

Range::Range() {
    invalidate();
}

Range::Range(qreal min, qreal max) {
    set(min, max);
}

Range Range::infinite() {
    return Range(-INF, +INF);
}

int Range::compare(const Range& that) const {
    debug::ensure(isValid() && that.isValid());
    RET_COMPARE_VALUE(min)
    RET_COMPARE_VALUE(max)
    return 0;
}

VALID_EQ_NE_OPERATOR(Range)

void Range::invalidate() {
    set(NAN, NAN);
}

bool Range::isValid() const {
    return !qIsNaN(min) && !qIsNaN(max);
}

bool Range::isEmpty() const {
    return !isValid() || min >= max;
}

qreal Range::width() const {
    return isValid() ? max - min : NAN;
}

qreal Range::center() const {
    return isValid() ? (min + max) / 2 : NAN;
}

void Range::set(qreal min_, qreal max_) {
    min = min_;
    max = max_;
    debug::ensure(!isValid() || min <= max);
}

void Range::safeSet(qreal v1, qreal v2) {
    if (v1 > v2)
        qSwap(v1, v2);
    set(v1, v2);
}

Range Range::safeFrom(qreal v1, qreal v2) {
    Range range;
    range.safeSet(v1, v2);
    return range;
}

void Range::extendBy(qreal val) {
    min = qIsNaN(min) ? val : qMin(min, val);
    max = qIsNaN(max) ? val : qMax(max, val);
}

void Range::extendBy(const Range& that) {
    extendBy(that.min);
    extendBy(that.max);
}

bool Range::contains(qreal val) const {
    return min <= val && val <= max;
}

bool Range::contains(const Range& that) const {
    return min <= that.min && that.max <= max;
}

bool Range::intersects(const Range& that) const {
    return min <= that.max && that.min <= max;
}

Range Range::intersect(const Range& that) const {
    if (!isValid() || !that.isValid())
        return Range();

    auto min_ = qMax(min, that.min), max_ = qMin(max, that.max);
    if (min_ <= max_)
        return Range(min_, max_);

    // disjoint
    auto val = that.min < min ? min : max;
    return Range(val, val); // empty, isValid()
}

qreal Range::bound(qreal value) const {
    if (isValid() && !qIsNaN(value))
        return qBound(min, value, max);
    return NAN;
}

QJsonObject Range::to_json() const {
    return { { "min", qreal_to_json(min) }, { "max", qreal_to_json(max) } };
}

void Range::from_json(JsonObj const& obj) THROWS {
    min = obj.loadQreal("min");
    max = obj.loadQreal("max");
}


// ************************************************************************** //
//  class Ranges
// ************************************************************************** //

bool Ranges::add(const Range& range) {
    vec<Range> newRanges;

    auto addRange = range;
    for (const Range& r : ranges_) {
        if (r.contains(range))
            return false;
        if (!range.contains(r)) {
            if (range.intersects(r))
                addRange.extendBy(r);
            else
                newRanges.append(r);
        }
    }

    newRanges.append(addRange);
    ranges_ = newRanges;
    sort();

    return true;
}

bool Ranges::rem(const Range& remRange) {
    vec<Range> newRanges;
    bool changed = false;

    for (const Range& r : ranges_) {
        if (!r.intersect(remRange).isEmpty()) {
            changed = true;
            if (r.min < remRange.min)
                newRanges.append(Range(r.min, remRange.min));
            if (r.max > remRange.max)
                newRanges.append(Range(remRange.max, r.max));
        } else {
            newRanges.append(r);
        }
    }

    if (changed)
        ranges_ = newRanges;
    return changed;
}

static bool lessThan(const Range& r1, const Range& r2) {
    if (r1.min < r2.min)
        return true;
    if (r1.min > r2.min)
        return false;
    return r1.max < r2.max;
}

void Ranges::sort() {
    std::sort(ranges_.begin(), ranges_.end(), lessThan);
}

QJsonArray Ranges::to_json() const {
    QJsonArray arr;
    for (const Range& range : ranges_)
        arr.append(range.to_json());
    return arr;
}

void Ranges::from_json(QJsonArray const& arr) THROWS {
    for_i (arr.count()) {
        Range range;
        range.from_json(arr.at(i).toObject());
        ranges_.append(range);
    }
}

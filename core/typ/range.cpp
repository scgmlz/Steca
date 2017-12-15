// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/range.cpp
//! @brief     Implements classes Range and Ranges.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test006_range (good coverage since this is somewhat involved)
//
// ************************************************************************** //

#include "def/comparators.h"
#include "def/idiomatic_for.h"
#include "typ/json.h"
#include "typ/range.h"
#include "typ/vec.h"

namespace typ {

Range::Range() {
    invalidate();
}

Range::Range(qreal val) {
    set(val);
}

Range::Range(qreal min, qreal max) {
    set(min, max);
}

Range Range::infinite() {
    return Range(-INF, +INF);
}

int Range::compare(Range const& that) const {
    debug::ensure(isValid() && that.isValid());
    RET_COMPARE_VALUE(min)
    RET_COMPARE_VALUE(max)
    return 0;
}

VALID_EQ_NE_OPERATOR(Range)

void Range::invalidate() {
    set(NAN);
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

void Range::set(qreal val) {
    set(val, val);
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

void Range::extendBy(Range const& that) {
    extendBy(that.min);
    extendBy(that.max);
}

bool Range::contains(qreal val) const {
    return min <= val && val <= max;
}

bool Range::contains(Range const& that) const {
    return min <= that.min && that.max <= max;
}

bool Range::intersects(Range const& that) const {
    return min <= that.max && that.min <= max;
}

Range Range::intersect(Range const& that) const {
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

JsonObj Range::saveJson() const {
    return JsonObj().saveQreal(json_key::MIN, min).saveQreal(json_key::MAX, max);
}

void Range::loadJson(JsonObj const& obj) THROWS {
    min = obj.loadQreal(json_key::MIN);
    max = obj.loadQreal(json_key::MAX);
}

Ranges::Ranges() {}

bool Ranges::add(Range const& range) {
    vec<Range> newRanges;

    auto addRange = range;
    for (Range const& r : ranges_) {
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

bool Ranges::rem(Range const& remRange) {
    vec<Range> newRanges;
    bool changed = false;

    for (Range const& r : ranges_) {
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

static bool lessThan(Range const& r1, Range const& r2) {
    if (r1.min < r2.min)
        return true;
    if (r1.min > r2.min)
        return false;
    return r1.max < r2.max;
}

void Ranges::sort() {
    std::sort(ranges_.begin(), ranges_.end(), lessThan);
}

JsonArr Ranges::saveJson() const {
    JsonArr arr;

    for (auto& range : ranges_)
        arr.append(range.saveJson());

    return arr;
}

void Ranges::loadJson(JsonArr const& arr) THROWS {
    for_i (arr.count()) {
        Range range;
        range.loadJson(arr.objAt(i));
        ranges_.append(range);
    }
}

} // namespace typ

// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/typ/typ_range.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ_range.h"
#include "def/def_cmp_impl.h"
#include "def/defs.h"
#include "typ/typ_json.h"
#include "wrap_doctest.h"

namespace typ {

#ifdef TESTS
static bool RANGE_EQ(Range::rc r1, Range::rc r2) {
    return r1.min == r2.min && r1.max == r2.max;
}

TEST_CASE("Range::EQ") {
    Range r1(1, 2), r2(r1);
    CHECK(RANGE_EQ(r1, r2));
}

#endif

Range::Range() {
    invalidate();
}

TEST("Range()", ({
         Range r;
         CHECK(qIsNaN(r.min));
         CHECK(qIsNaN(r.max));
     });)

Range::Range(qreal val) {
    set(val);
}

TEST("Range(v)", ({
         Range r(6);
         CHECK_EQ(6, r.min);
         CHECK_EQ(6, r.max);
     });)

Range::Range(qreal min, qreal max) {
    set(min, max);
}

TEST("Range(min, max)", ({
         Range r(6, 7);
         CHECK_EQ(6, r.min);
         CHECK_EQ(7, r.max);
     });)

Range Range::infinite() {
    return Range(-INF, +INF);
}

int Range::compare(rc that) const {
    EXPECT(isValid() && that.isValid())
    RET_COMPARE_VALUE(min)
    RET_COMPARE_VALUE(max)
    return 0;
}

VALID_EQ_NE_OPERATOR(Range)

TEST("Range::infinite", ({
         auto r = Range::infinite();
         CHECK(qIsInf(r.min));
         CHECK_LE(r.min, 0);
         CHECK(qIsInf(r.max));
         CHECK_GE(r.max, 0);
     });)

void Range::invalidate() {
    set(NAN);
}

bool Range::isValid() const {
    return !qIsNaN(min) && !qIsNaN(max);
}

TEST("Range::valid", ({
         Range r;
         CHECK(!r.isValid());
         r.min = 0;
         CHECK(!r.isValid());
         r.max = 0;
         CHECK(r.isValid());
         r.invalidate();
         CHECK(!r.isValid());
         r.set(0);
         CHECK(r.isValid());
     });)

bool Range::isEmpty() const {
    return !isValid() || min >= max;
}

TEST("Range::empty", ({
         CHECK(Range().isEmpty());
         CHECK(!Range::infinite().isEmpty());

         Range r(0);
         CHECK(r.isEmpty());
         r.max = 1;
         CHECK(!r.isEmpty());
         r.min = 2;
         CHECK(r.isEmpty());
         r.max = NAN;
         CHECK(r.isEmpty());
     });)

qreal Range::width() const {
    return isValid() ? max - min : NAN;
}

TEST("Range::width", ({
         CHECK(qIsNaN(Range().width()));
         CHECK_EQ(0, Range(0).width());
         CHECK(qIsInf(Range(0, INF).width()));
         CHECK(qIsInf(Range::infinite().width()));

         Range r(0);
         CHECK(r.isEmpty());
         r.max = 1;
         CHECK(!r.isEmpty());
         r.min = 2;
         CHECK(r.isEmpty());
         r.max = NAN;
         CHECK(r.isEmpty());
     });)

qreal Range::center() const {
    return isValid() ? (min + max) / 2 : NAN;
}

TEST("Range::center", ({
         CHECK(qIsNaN(Range().center()));
         CHECK_EQ(0, Range(0).center());
         CHECK(qIsNaN(Range(0, NAN).center()));
         CHECK(qIsInf(Range(0, INF).center()));
         CHECK(qIsNaN(Range::infinite().center()));
     });)

void Range::set(qreal val) {
    set(val, val);
}

void Range::set(qreal min_, qreal max_) {
    min = min_;
    max = max_;
    EXPECT(!isValid() || min <= max)
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

TEST("Range::safe", ({
         auto r = Range::safeFrom(2, 3);
         RANGE_EQ(r, Range(2, 3));
         r = Range::safeFrom(3, 2);
         RANGE_EQ(r, Range(2, 3));
         r.safeSet(3, 4);
         RANGE_EQ(r, Range(3, 4));
         r.safeSet(4, 3);
         RANGE_EQ(r, Range(3, 4));
         r.safeSet(+INF, -INF);
         RANGE_EQ(r, Range::infinite());
     });)

void Range::extendBy(qreal val) {
    min = qIsNaN(min) ? val : qMin(min, val);
    max = qIsNaN(max) ? val : qMax(max, val);
}

void Range::extendBy(Range::rc that) {
    extendBy(that.min);
    extendBy(that.max);
}

TEST("Range::extend", ({
         auto r = Range(1, 2);
         r.extendBy(-1);
         RANGE_EQ(r, Range(-1, 2));
         r.extendBy(Range(3, 4));
         RANGE_EQ(r, Range(-1, 4));
     });)

bool Range::contains(qreal val) const {
    return min <= val && val <= max;
}

bool Range::contains(Range::rc that) const {
    return min <= that.min && that.max <= max;
}

TEST("Range::contains", ({
         auto r = Range(-1, +1);

         CHECK(!Range().contains(r));
         CHECK(!Range().contains(Range()));
         CHECK(!Range().contains(Range::infinite()));

         CHECK(Range::infinite().contains(r));
         CHECK(Range::infinite().contains(Range::infinite()));
         CHECK(!Range::infinite().contains(Range()));

         CHECK(!r.contains(Range()));
         CHECK(!r.contains(Range::infinite()));
         CHECK(!r.contains(NAN));
         CHECK(!r.contains(INF));

         CHECK(r.contains(r));

         CHECK(r.contains(-1));
         CHECK(r.contains(0));
         CHECK(r.contains(+1));
         CHECK(r.contains(Range(0, 1)));
         CHECK(!r.contains(Range(-2, 0)));
     });)

bool Range::intersects(Range::rc that) const {
    return min <= that.max && that.min <= max;
}

TEST("Range::intersects", ({
         auto r = Range(-1, +1);

         CHECK(!Range().intersects(r));
         CHECK(!Range().intersects(Range()));
         CHECK(!Range().intersects(Range::infinite()));

         CHECK(Range::infinite().intersects(r));
         CHECK(Range::infinite().intersects(Range::infinite()));
         CHECK(!Range::infinite().intersects(Range()));

         CHECK(!r.intersects(Range()));
         CHECK(r.intersects(Range::infinite()));

         CHECK(r.intersects(r));

         CHECK(r.intersects(Range(0, 10)));
         CHECK(r.intersects(Range(-2, 0)));
     });)

Range Range::intersect(Range::rc that) const {
    if (!isValid() || !that.isValid())
        return Range();

    auto min_ = qMax(min, that.min), max_ = qMin(max, that.max);
    if (min_ <= max_)
        return Range(min_, max_);

    // disjoint
    auto val = that.min < min ? min : max;
    return Range(val, val); // empty, isValid()
}

TEST("Range::intersect", ({
         auto r = Range(-1, +1);

         CHECK(!Range().intersect(r).isValid());
         CHECK(!r.intersect(Range()).isValid());
         CHECK(!Range().intersect(Range::infinite()).isValid());

         CHECK_EQ(r, Range::infinite().intersect(r));
         CHECK_EQ(Range::infinite(), Range::infinite().intersect(Range::infinite()));
         CHECK(!Range::infinite().intersect(Range()).isValid());

         CHECK(!r.intersect(Range()).isValid());
         CHECK_EQ(r, r.intersect(Range::infinite()));

         CHECK_EQ(r, r.intersect(r));

         CHECK_EQ(Range(0, 1), r.intersect(Range(0, 10)));
         CHECK_EQ(Range(-1, 0), r.intersect(Range(-2, 0)));

         auto disjoint = Range(-3, -2);
         CHECK(r.intersect(disjoint).isEmpty());
         CHECK_EQ(r.min, r.intersect(disjoint).min);
     });)

qreal Range::bound(qreal value) const {
    if (isValid() && !qIsNaN(value))
        return qBound(min, value, max);
    return NAN;
}

TEST("Range::bound", ({
         auto r = Range(-1, +1);

         CHECK(qIsNaN(Range().bound(0)));
         CHECK(qIsNaN(Range().bound(INF)));
         CHECK(qIsNaN(Range().bound(NAN)));
         CHECK_EQ(0, Range::infinite().bound(0));
         CHECK(qIsInf(Range::infinite().bound(INF)));
         CHECK(qIsNaN(Range::infinite().bound(NAN)));

         CHECK_EQ(0, r.bound(0));
         CHECK_EQ(-1, r.bound(-10));
         CHECK_EQ(-1, r.bound(-INF));
         CHECK_EQ(+1, r.bound(+10));
         CHECK_EQ(+1, r.bound(+INF));
     });)

JsonObj Range::saveJson() const {
    return JsonObj().saveQreal(json_key::MIN, min).saveQreal(json_key::MAX, max);
}

void Range::loadJson(JsonObj::rc obj) THROWS {
    min = obj.loadQreal(json_key::MIN);
    max = obj.loadQreal(json_key::MAX);
}

TEST("Range::json", ({
         Range r(-1, 2), r1;
         r1.loadJson(r.saveJson());
         CHECK_EQ(r, r1);
     });)

Ranges::Ranges() {}

bool Ranges::add(Range::rc range) {
    vec<Range> newRanges;

    auto addRange = range;
    for (Range::rc r : ranges_) {
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

bool Ranges::rem(Range::rc remRange) {
    vec<Range> newRanges;
    bool changed = false;

    for (Range::rc r : ranges_) {
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

static bool lessThan(Range::rc r1, Range::rc r2) {
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

void Ranges::loadJson(JsonArr::rc arr) THROWS {
    for_i (arr.count()) {
        Range range;
        range.loadJson(arr.objAt(i));
        ranges_.append(range);
    }
}

#ifdef TESTS

typedef struct { qreal min, max; } min_max;

static bool RANGES_EQ(Ranges::rc rs, vec<min_max> mm) {
    if (rs.count() != mm.count())
        return false;

    for_i (rs.count()) {
        auto r = rs.at(i);
        auto m = mm.at(i);
        if (r.min != m.min || r.max != m.max)
            return false;
    }

    return true;
}

static bool RANGES_EQ(Ranges::rc rs1, Ranges::rc rs2) {
    if (rs1.count() != rs2.count())
        return false;

    for_i (rs1.count()) {
        if (rs1.at(i) != rs2.at(i))
            return false;
    }

    return true;
}

#endif

TEST("Ranges", ({
         Ranges rs;
         REQUIRE(rs.isEmpty());
         CHECK(RANGES_EQ(rs, rs));
         CHECK(RANGES_EQ(rs, Ranges()));

         Range r1(0, 1), r2(1, 2), r3(2, 3), r4(3, 4);

         REQUIRE(rs.add(r4));
         CHECK_FALSE(rs.add(r4));
         CHECK(RANGES_EQ(rs, { { 3, 4 } }));

         REQUIRE(rs.add(r1));
         CHECK_FALSE(rs.add(r1));
         CHECK(RANGES_EQ(rs, { { 0, 1 }, { 3, 4 } }));

         REQUIRE(rs.add(r2));
         CHECK(RANGES_EQ(rs, { { 0, 2 }, { 3, 4 } }));

         REQUIRE(rs.add(r3));
         CHECK(RANGES_EQ(rs, { { 0, 4 } }));

         REQUIRE(rs.rem(r2));
         CHECK_FALSE(rs.rem(r2));
         CHECK(RANGES_EQ(rs, { { 0, 1 }, { 2, 4 } }));

         rs.clear();
         CHECK_FALSE(rs.rem(r1));
     });)

TEST("Ranges::json", ({
         Ranges rs, rs1;
         rs.add(Range());
         rs.add(Range(9));
         rs.add(Range(-3, -2));
         rs.add(Range::infinite());

         rs1.loadJson(rs.saveJson());
         CHECK(RANGES_EQ(rs, rs1));
     });)
}

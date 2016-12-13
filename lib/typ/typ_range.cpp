/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "typ_range.h"
#include "def/defs.h"
#include "typ/typ_json.h"
#include <qmath.h>
#include "test/tests.h"

namespace typ {
//------------------------------------------------------------------------------

#ifdef TESTS
static bool RANGE_EQ(Range::rc r1, Range::rc r2) {
  return r1.min == r2.min && r1.max == r2.max;
}

TEST_CASE("Range::EQ") {
  Range r1(1,2), r2(r1);
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
  Range r(6,7);
  CHECK_EQ(6, r.min);
  CHECK_EQ(7, r.max);
});)

Range Range::infinite() {
  return Range(-qInf(), +qInf());
}

TEST("Range::infinite", ({
  auto r = Range::infinite();
  CHECK(qIsInf(r.min)); CHECK_LE(r.min, 0);
  CHECK(qIsInf(r.max)); CHECK_GE(r.max, 0);
});)

void Range::invalidate() {
  set(qQNaN());
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
  r.max = qQNaN();
  CHECK(r.isEmpty());
});)

qreal Range::width() const {
  return isValid() ? max - min : qQNaN();
}

TEST("Range::width", ({
  CHECK(qIsNaN(Range().width()));
  CHECK_EQ(0, Range(0).width());
  CHECK(qIsInf(Range(0,qInf()).width()));
  CHECK(qIsInf(Range::infinite().width()));

  Range r(0);
  CHECK(r.isEmpty());
  r.max = 1;
  CHECK(!r.isEmpty());
  r.min = 2;
  CHECK(r.isEmpty());
  r.max = qQNaN();
  CHECK(r.isEmpty());
});)

qreal Range::center() const {
  return isValid() ? (min + max) / 2 : qQNaN();
}

TEST("Range::center", ({
  CHECK(qIsNaN(Range().center()));
  CHECK_EQ(0, Range(0).center());
  CHECK(qIsNaN(Range(0,qQNaN()).center()));
  CHECK(qIsInf(Range(0,qInf()).center()));
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
  if (v1 > v2) qSwap(v1, v2);
  set(v1, v2);
}

Range Range::safeFrom(qreal v1, qreal v2) {
  Range range;
  range.safeSet(v1, v2);
  return range;
}

TEST("Range::safe", ({
  auto r = Range::safeFrom(2,3);
  RANGE_EQ(r, Range(2,3));
  r = Range::safeFrom(3,2);
  RANGE_EQ(r, Range(2,3));
  r.safeSet(3,4);
  RANGE_EQ(r, Range(3,4));
  r.safeSet(4,3);
  RANGE_EQ(r, Range(3,4));
  r.safeSet(+qInf(), -qInf());
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
  auto r  = Range(1,2);
  r.extendBy(-1);
  RANGE_EQ(r, Range(-1,2));
  r.extendBy(Range(3,4));
  RANGE_EQ(r, Range(-1,4));
});)

bool Range::contains(qreal val) const {
  return min <= val && val <= max;
}

bool Range::contains(Range::rc that) const {
  return min <= that.min && that.max <= max;
}

bool Range::intersects(Range::rc that) const {
  return min <= that.max && that.min <= max;
}

Range Range::intersect(Range::rc that) const {
  if (isValid() || !that.isValid()) {
    auto min_ = qMax(min, that.min), max_ = qMin(max, that.max);
    if (min_ <= max_)
      return Range(min_, max_);
    return Range(min, min);
  }

  return Range();
}

qreal Range::bound(qreal value) const {
  if (isValid()) value = qBound(min, value, max);
  return value;
}

JsonObj Range::saveJson() const {
  return JsonObj().saveQreal(json_key::MIN, min).saveQreal(json_key::MAX, max);
}

void Range::loadJson(JsonObj::rc obj) THROWS {
  min = obj.loadQreal(json_key::MIN);
  max = obj.loadQreal(json_key::MAX);
}

#ifndef QT_NO_DEBUG
QDebug& operator<<(QDebug& d, Range::rc rge) {
  d << '<' << rge.min << '-' << rge.max << '>';
  return d;
}
#endif

//------------------------------------------------------------------------------

Ranges::Ranges() {
}

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
  bool           changed = false;

  for (Range::rc r : ranges_) {
    if (r.intersects(remRange)) {
      changed = true;
      if (r.min < remRange.min) newRanges.append(Range(r.min, remRange.min));
      if (r.max > remRange.max) newRanges.append(Range(remRange.max, r.max));
    } else {
      newRanges.append(r);
    }
  }

  if (changed) ranges_ = newRanges;
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

//------------------------------------------------------------------------------
}
// eof

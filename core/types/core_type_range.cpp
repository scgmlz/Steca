// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_range.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_type_range.h"
#include "core_json.h"

namespace core {
//------------------------------------------------------------------------------

Range::Range() {
  invalidate();
}

Range::Range(qreal val) {
  set(val);
}

Range::Range(qreal min, qreal max) {
  set(min,max);
}

Range Range::infinite() {
  return Range(-qInf(), +qInf());
}

void Range::invalidate() {
  set(qQNaN());
}

bool Range::isValid() const {
  return !qIsNaN(min) && !qIsNaN(max);
}

qreal Range::width() const {
  return isValid() ? max - min : qQNaN();
}

qreal Range::center() const {
  return isValid() ? (min + max) / 2 : qQNaN();
}

void Range::set(qreal val) {
  set(val,val);
}

void Range::set(qreal min_, qreal max_) {
  min = min_; max = max_;
  ASSERT(!isValid() || min <= max)
}

void Range::safeSet(qreal v1, qreal v2) {
  if (v1 > v2) qSwap(v1,v2);
  set(v1,v2);
}

Range Range::safeFrom(qreal v1, qreal v2) {
  Range range; range.safeSet(v1,v2);
  return range;
}

void Range::extendBy(qreal val) {
  min = qIsNaN(min) ? val : qMin(min,val);
  max = qIsNaN(max) ? val : qMax(max,val);
}

void Range::extendBy(Range const& that) {
  extendBy(that.min); extendBy(that.max);
}

bool Range::contains(qreal val) const {
  ASSERT(isValid())
  return min <= val && val <= max;
}

bool Range::contains(Range const& that) const {
  ASSERT(isValid() && that.isValid())
  return min <= that.min && that.max <= max;
}

bool Range::intersects(Range const& that) const {
  ASSERT(isValid() && that.isValid())
  return min <= that.max && that.min <= max;
}

qreal Range::bound(qreal value) const {
  if (isValid()) value = qBound(min,value,max);
  return value;
}

static str const KEY_MIN("min"), KEY_MAX("max");

void Range::loadJson(rcJsonObj obj) THROWS {
  min = obj.loadReal(KEY_MIN);
  max = obj.loadReal(KEY_MAX);
}

JsonObj Range::saveJson() const {
  return JsonObj()
    .saveReal(KEY_MIN, min)
    .saveReal(KEY_MAX, max);
}

//------------------------------------------------------------------------------

Ranges::Ranges() {
}

bool Ranges::add(Range const& range) {
  QVector<Range> newRanges;

  auto addRange = range;
  for (Range const& r: ranges) {
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
  ranges = newRanges;
  sort();

  return true;
}

bool Ranges::rem(Range const& remRange) {
  QVector<Range> newRanges;
  bool changed = false;

  for (Range const& r: ranges) {
    if (r.intersects(remRange)) {
      changed = true;
      if (r.min < remRange.min)
        newRanges.append(Range(r.min,remRange.min));
      if (r.max > remRange.max)
        newRanges.append(Range(remRange.max,r.max));
    } else {
      newRanges.append(r);
    }
  }

  if (changed) ranges = newRanges;
  return changed;
}

static bool lessThan(Range const& r1, Range const& r2) {
  if (r1.min < r2.min) return true;
  if (r1.min > r2.min) return false;
  return r1.max < r2.max;
}

void Ranges::sort() {
  std::sort(ranges.begin(),ranges.end(),lessThan);
}

static str const KEY_NUM("%1"), KEY_COUNT("count");

void Ranges::loadJson(rcJsonObj obj) THROWS {
  uint count = obj.loadUint(KEY_COUNT);
  for_i (count) {
    Range r;
    r.loadJson(obj[KEY_NUM.arg(i+1)].toObject());
    ranges.append(r);
  }
}

JsonObj Ranges::saveJson() const {
  JsonObj obj;

  uint count = ranges.count();
  obj.saveUint(KEY_COUNT,count);

  for_i (count)
    obj[KEY_NUM.arg(i+1)] = ranges.at(i).saveJson();

  return obj;
}

//------------------------------------------------------------------------------
}
// eof

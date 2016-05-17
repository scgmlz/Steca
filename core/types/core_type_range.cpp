// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
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

bool Range::isEmpty() const {
  return !isValid() || min >= max;
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
  EXPECT(!isValid() || min <= max)
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

void Range::extendBy(rcRange that) {
  extendBy(that.min); extendBy(that.max);
}

bool Range::contains(qreal val) const {
  ENSURE(isValid())
  return min <= val && val <= max;
}

bool Range::contains(rcRange that) const {
  ENSURE(isValid() && that.isValid())
  return min <= that.min && that.max <= max;
}

bool Range::intersects(rcRange that) const {
  ENSURE(isValid() && that.isValid())
  return min <= that.max && that.min <= max;
}

qreal Range::bound(qreal value) const {
  if (isValid()) value = qBound(min,value,max);
  return value;
}

static str const KEY_MIN("min"), KEY_MAX("max");

JsonObj Range::saveJson() const {
  return JsonObj()
    .saveReal(KEY_MIN, min)
    .saveReal(KEY_MAX, max);
}

void Range::loadJson(rcJsonObj obj) THROWS {
  min = obj.loadReal(KEY_MIN);
  max = obj.loadReal(KEY_MAX);
}

//------------------------------------------------------------------------------

Ranges::Ranges() {
}

bool Ranges::add(rcRange range) {
  QVector<Range> newRanges;

  auto addRange = range;
  for (rcRange r: ranges_) {
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

bool Ranges::rem(rcRange remRange) {
  QVector<Range> newRanges;
  bool changed = false;

  for (rcRange r: ranges_) {
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

  if (changed) ranges_ = newRanges;
  return changed;
}

static bool lessThan(rcRange r1, rcRange r2) {
  if (r1.min < r2.min) return true;
  if (r1.min > r2.min) return false;
  return r1.max < r2.max;
}

void Ranges::sort() {
  std::sort(ranges_.begin(),ranges_.end(),lessThan);
}

JsonArr Ranges::saveJson() const {
  JsonArr arr;

  for (auto &range: ranges_)
    arr.append(range.saveJson());

  return arr;
}

void Ranges::loadJson(rcJsonArr arr) THROWS {
  for_i (arr.count()) {
    Range range; range.loadJson(arr.objAt(i));
    ranges_.append(range);
  }
}

//------------------------------------------------------------------------------
}
// eof

// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_range.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "typ_range.h"
#include "def/defs.h"
#include "typ/typ_json.h"
#include <qmath.h>

namespace typ {
//------------------------------------------------------------------------------

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

void Range::extendBy(qreal val) {
  min = qIsNaN(min) ? val : qMin(min, val);
  max = qIsNaN(max) ? val : qMax(max, val);
}

void Range::extendBy(Range::rc that) {
  extendBy(that.min);
  extendBy(that.max);
}

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

static str const KEY_MIN("min"), KEY_MAX("max");

JsonObj Range::saveJson() const {
  return JsonObj().saveQreal(KEY_MIN, min).saveQreal(KEY_MAX, max);
}

void Range::loadJson(JsonObj::rc obj) THROWS {
  min = obj.loadQreal(KEY_MIN);
  max = obj.loadQreal(KEY_MAX);
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

  for (auto &range : ranges_)
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

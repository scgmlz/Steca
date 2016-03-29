// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_types.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_types.h"
#include "core_json.h"

#include <algorithm>
#include <QSize>
#include <qmath.h>
#include <numeric>
#include <QStringList>

namespace core {

//------------------------------------------------------------------------------

XY::XY(): x(qQNaN()), y(qQNaN()) {
}

XY::XY(qreal x_, qreal y_): x(x_), y(y_) {
}

bool XY::isDefined() const {
  return !qIsNaN(x) && !qIsNaN(y);
}

static str const KEY_X("X"), KEY_Y("Y");

void XY::loadJson(rcJsonObj obj) THROWS {
  x = obj.loadReal(KEY_X);
  y = obj.loadReal(KEY_Y);
}

JsonObj XY::saveJson() const {
  JsonObj obj;
  obj.saveReal(KEY_X, x);
  obj.saveReal(KEY_Y, y);
  return obj;
}

//------------------------------------------------------------------------------

DiffractionAngles::DiffractionAngles(qreal gamma_, qreal tth_)
: gamma(gamma_), tth(tth_) {
}

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
  return isValid() ? max-min : qQNaN();
}

qreal Range::center() const {
  return isValid() ? width() / 2 : qQNaN();
}

void Range::set(qreal val) {
  set(val,val);
}

void Range::set(qreal min_, qreal max_) {
  ASSERT((qIsNaN(min_) && qIsNaN(max_)) || min_ <= max_)
  min = min_; max = max_;
}

void Range::safeSet(qreal v1, qreal v2) {
  if (v1 < v2) set(v1,v2);
  else         set(v2,v1);
}

Range Range::safeFrom(qreal v1, qreal v2) {
  Range range; range.safeSet(v1,v2);
  return range;
}

void Range::extend(qreal val) {
  min = qIsNaN(min) ? val : qMin(min,val);
  max = qIsNaN(max) ? val : qMax(max,val);
}

void Range::extend(Range const& that) {
  extend(that.min); extend(that.max);
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
  JsonObj obj;
  obj.saveReal(KEY_MIN, min);
  obj.saveReal(KEY_MAX, max);
  return obj;
}

//------------------------------------------------------------------------------

Ranges::Ranges() {
}

void Ranges::clear() {
  ranges.clear();
}

bool Ranges::isEmpty() const {
  return ranges.isEmpty();
}

uint Ranges::count() const {
  return ranges.count();
}

Range const& Ranges::at(uint i) const {
  return ranges.at(i);
}

bool Ranges::add(Range const& range) {
  QVector<Range> newRanges;

  auto addRange = range;
  for (Range const& r: ranges) {
    if (r.contains(range))
      return false;
    if (!range.contains(r)) {
      if (range.intersects(r))
        addRange.extend(r);
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

static str const
  KEY_RANGE_NUM("r%1"),
  KEY_RANGES("ranges"),
  KEY_RANGE_COUNT("rangeCount");

void Ranges::loadJson(rcJsonObj obj) THROWS { // REVIEW (dispose of KEY_RANGES?)
  JsonObj rsObj = obj[KEY_RANGES].toObject();
  int size = rsObj[KEY_RANGE_COUNT].toInt();
  for_i (size) {
    JsonObj rObj;
    rObj = rsObj[KEY_RANGE_NUM.arg(i+1)].toObject();
    Range r;
    r.loadJson(rObj);
    ranges.append(r);
  }
}

JsonObj Ranges::saveJson() const { // REVIEW
  JsonObj obj;

  obj[KEY_RANGE_COUNT] = ranges.size();
  for_i (ranges.count())
    obj[KEY_RANGE_NUM.arg(i+1)] = ranges.at(i).saveJson();
  obj[KEY_RANGES] = obj;

  return obj;
}

//------------------------------------------------------------------------------

qreal radToDeg(qreal rad) {
  return rad * (180 / M_PI);
}

qreal degToRad(qreal deg) {
  return deg * (M_PI / 180);
}

//------------------------------------------------------------------------------

str_lst getStringListNormalization() {
  return str_lst{"Disable","Mesurment time","Monitor counts","Background level"};
}

//------------------------------------------------------------------------------
}
// eof

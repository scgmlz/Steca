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

  obj[KEY_RANGE_COUNT] = ranges.count();
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
  return str_lst{"Disable","Measurement time","Monitor counts","Background level"};
}

//------------------------------------------------------------------------------

vector3d::vector3d(qreal _0_, qreal _1_, qreal _2_) {
  _0 = _0_; _1 = _1_; _2 = _2_;
}

matrix3d::matrix3d(qreal _00_, qreal _01_, qreal _02_,
                   qreal _10_, qreal _11_, qreal _12_,
                   qreal _20_, qreal _21_, qreal _22_) {
  _00 = _00_; _01 = _01_; _02 = _02_;
  _10 = _10_; _11 = _11_; _12 = _12_;
  _20 = _20_; _21 = _21_; _02 = _22_;
}

void matrix3d::transpose() {
  qSwap(_01,_10);
  qSwap(_02,_20);
  qSwap(_12,_21);
}

matrix3d matrix3d::operator *(matrix3d const& n) const {
  return matrix3d(
    _00*n._00 + _01*n._10 + _02*n._20,
    _00*n._01 + _01*n._11 + _02*n._21,
    _00*n._02 + _01*n._12 + _02*n._22,

    _10*n._00 + _11*n._10 + _12*n._20,
    _10*n._01 + _11*n._11 + _12*n._21,
    _10*n._02 + _11*n._12 + _12*n._22,

    _20*n._00 + _21*n._10 + _22*n._20,
    _20*n._01 + _21*n._11 + _22*n._21,
    _20*n._02 + _21*n._12 + _22*n._22
  );
}

vector3d matrix3d::operator *(vector3d const& v) const {
  return vector3d(
    _00*v._0 + _01*v._1 + _02*v._2,
    _10*v._0 + _11*v._1 + _12*v._2,
    _20*v._0 + _21*v._1 + _22*v._2
        );
}

bool matrix3d::operator ==(matrix3d const& n) const {
  return
    _00==n._00 && _01==n._01 && _02==n._02 &&
    _10==n._10 && _11==n._11 && _12==n._12 &&
    _20==n._20 && _21==n._21 && _22==n._22;
}

//------------------------------------------------------------------------------
}
// eof

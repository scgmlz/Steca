#include "core_types.h"

#include <algorithm>
#include <QSize>
#include <qmath.h>
#include <numeric>
#include <QJsonObject>

namespace core {
//------------------------------------------------------------------------------

qreal deg_rad(qreal rad) {
  return rad * (180 / M_PI);
}

qreal rad_deg(qreal deg) {
  return deg * (M_PI / 180);
}

//------------------------------------------------------------------------------

XY::XY(): x(qQNaN()), y(qQNaN()) {
}

XY::XY(qreal x_, qreal y_): x(x_), y(y_) {
}

bool XY::isDefined() const {
  return !qIsNaN(x) && !qIsNaN(y);
}

static str KEY_X("X");
static str KEY_Y("Y");

void XY::loadFrom(QJsonObject const& obj) THROWS {
  x = loadReal(obj,KEY_X);
  y = loadReal(obj,KEY_Y);
}

void XY::saveTo(QJsonObject &obj) const {
  obj[KEY_X] = x;
  obj[KEY_Y] = y;
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

qreal Range::width() const {
  return isValid() ? max-min : qQNaN();
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

static str KEY_RANGE_MIN("min");
static str KEY_RANGE_MAX("max");

void Range::loadFrom(QJsonObject const& obj) {
  min = loadReal(obj, KEY_RANGE_MIN);
  max = loadReal(obj, KEY_RANGE_MAX);
}

void Range::saveTo(QJsonObject& obj) const {
  saveReal(obj, KEY_RANGE_MIN, min);
  saveReal(obj, KEY_RANGE_MAX, max);
}

//------------------------------------------------------------------------------

Ranges::Ranges() {
}

void Ranges::clear() {
  ranges.clear();
}

bool Ranges::add(Range const& range) {
  QVector<Range> newRanges;

  auto newRange = range;
  for (auto &r: ranges) {
    if (r.contains(range))
      return false;
    if (!range.contains(r)) {
      if (range.intersects(r))
        newRange.extend(r);
      else
        newRanges.append(r);
    }
  }

  newRanges.append(newRange);
  ranges = newRanges;
  sort();

  return true;
}

bool Ranges::rem(Range const& range) {
  QVector<Range> newRanges;

  bool change = false;
  for (auto &r: ranges) {
    if (r.intersects(range)) {
      change = true;
      if (r.min < range.min)
        newRanges.append(Range(r.min,range.min));
      if (r.max > range.max)
        newRanges.append(Range(range.max,r.max));
    } else {
      newRanges.append(r);
    }
  }

  if (change) ranges = newRanges;
  return change;
}

static bool lessThan(Range const& r1, Range const& r2) {
  if (r1.min < r2.min) return true;
  if (r1.min > r2.min) return false;
  return r1.max < r2.max;
}

void Ranges::sort() {
  std::sort(ranges.begin(),ranges.end(),lessThan);
}

static str KEY_RANGE_NUM("r%1");
static str KEY_RANGES("ranges");
static str KEY_RANGE_COUNT("rangeCount");

void Ranges::loadFrom(QJsonObject const& obj) {
  QJsonObject rsObj = obj[KEY_RANGES].toObject();
  int size = rsObj[KEY_RANGE_COUNT].toInt();
  for_i (size) {
    QJsonObject rObj;
    rObj = rsObj[str(KEY_RANGE_NUM).arg(i+1)].toObject();
    Range r;
    r.loadFrom(rObj);
    ranges.append(r);
  }
}

void Ranges::saveTo(QJsonObject& obj) const {
  QJsonObject rsObj;
  rsObj[KEY_RANGE_COUNT] = ranges.size();
  for_i (ranges.count()) {
    QJsonObject rObj;
    ranges.at(i).saveTo(rObj);
    rsObj[str(KEY_RANGE_NUM).arg(i+1)] = rObj;
  }
  obj[KEY_RANGES] = rsObj;
}

//------------------------------------------------------------------------------

Curve::Curve() = default;

void Curve::clear() {
  xs.clear(); ys.clear();
  xRange.invalidate();
  yRange.invalidate();
}

bool Curve::isEmpty() const {
  return xs.isEmpty();
}

uint Curve::count() const {
  ASSERT(xs.count() == ys.count())
  return xs.count();
}

bool Curve::isOrdered() const {
  return std::is_sorted(xs.cbegin(), xs.cend());
}

void Curve::append(qreal x, qreal y) {
  xs.append(x);
  ys.append(y);
  xRange.extend(x);
  yRange.extend(y);
}

Curve Curve::intersect(Range const& range) const {
  Curve res;

  ASSERT(isOrdered())

  uint xi = 0, cnt = count();
  auto minX = range.min, maxX = range.max;
  while (xi<cnt && xs[xi] < minX)
    ++xi;
  while (xi<cnt && xs[xi] <= maxX) {
    res.append(xs[xi],ys[xi]);
    ++xi;
  }

  return res;
}

Curve Curve::intersect(Ranges const& ranges) const {
  Curve res;

  // collect points that are in ranges
  // it works because both curve points and ranges are ordered and ranges are non-overlapping
  ASSERT(isOrdered())

  uint xi = 0, cnt = count();
  for_i (ranges.count()) {
    Range const& range = ranges.at(i);
    auto minX = range.min, maxX = range.max;
    while (xi<cnt && xs[xi] < minX)
      ++xi;
    while (xi<cnt && xs[xi] <= maxX) {
      res.append(xs[xi],ys[xi]);
      ++xi;
    }
  }

  return res;
}

Curve Curve::smooth() const {
  // moving average, 3 points
  Curve res;

  for (int i = 1, cnt = count(); i+1 < cnt; ++i) {
    res.append(xs[i], (ys[i-1] + ys[i] + ys[i+1])/3.);
  }

  return res;
}

uint Curve::maxYindex() const {
  if (isEmpty()) return 0;

  auto yMax = ys[0]; uint index = 0;
  for_i (count()) {
    auto y = ys[i];
    if (y > yMax) {
      yMax = y; index = i;
    }
  }

  return index;
}

//------------------------------------------------------------------------------

static str const INF_P("+inf"), INF_M("-inf");

qreal loadReal(QJsonObject const& obj, rcstr tag) THROWS {
  auto val = obj[tag];

  switch (val.type()) {
  case QJsonValue::Undefined:
    return qQNaN();
  case QJsonValue::String: {
    auto s = val.toString();
    if (INF_P == s) return +qInf();
    if (INF_M == s) return -qInf();
    THROW("bad number format");
  }
  default:
    return val.toDouble();
  }
}

void saveReal(QJsonObject& obj, rcstr tag, qreal num) {
  if (qIsNaN(num)) return; // do not save anything

  auto val = obj[tag];
  if (qIsInf(num))
    val = (num < 0) ? INF_M : INF_P;
  else
    val = num;
}

bool areEqual(qreal r1, qreal r2) {
  return
    (qIsNaN(r1) && qIsNaN(r2)) || r1 == r2;
}


//------------------------------------------------------------------------------
}
// eof

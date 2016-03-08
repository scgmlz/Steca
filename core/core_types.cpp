#include "core_types.h"

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

static str KEY_RANGE_MIN("range_min");
static str KEY_RANGE_MAX("range_max");
static str KEY_RANGE("range");

void Range::loadFrom(QJsonObject const& obj) {
  QJsonObject rObj = obj[KEY_RANGE].toObject();
  min = rObj[KEY_RANGE_MIN].toDouble();
  max = rObj[KEY_RANGE_MAX].toDouble();
}

void Range::saveTo(QJsonObject& obj) const {
  QJsonObject rObj;
  rObj[KEY_RANGE_MIN] = min;
  rObj[KEY_RANGE_MAX] = max;
  obj[KEY_RANGE] = rObj;
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
  for_i (ranges.size()) {
    QJsonObject rObj;
    ranges.at(i).saveTo(rObj);
    rsObj[str(KEY_RANGE_NUM).arg(i+1)] = rObj;
  }
  obj[KEY_RANGES] = rsObj;
}

//------------------------------------------------------------------------------

Curve::Curve() {
}

Curve::~Curve() {
}

void Curve::clear() {
  xs.clear(); ys.clear();
}

bool Curve::isEmpty() const {
  return xs.isEmpty();
}

uint Curve::count() const {
  ASSERT(xs.count() == ys.count())
  return xs.count();
}

bool Curve::isOrdered() const {
  qreal lastX = -qInf();
  for (qreal x: xs) {
    if (lastX >= x) return false;
    lastX = x;
  }

  return true;
}

void Curve::append(qreal x, qreal y) {
  xs.append(x);
  ys.append(y);
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

TI_Curve::TI_Curve() {
}

void TI_Curve::clear() {
  super::clear();
  tthRange.invalidate();
  intenRange.invalidate();
}

void TI_Curve::append(qreal tth_, qreal inten_) {
  super::append(tth_,inten_);
  tthRange.extend(tth_);
  intenRange.extend(inten_);
}
//------------------------------------------------------------------------------

qreal loadReal(QJsonObject const& obj, rcstr tag) {
  if(obj[tag] == QJsonValue::Undefined){
    return qQNaN();
  }
  return obj[tag].toDouble();
}
//------------------------------------------------------------------------------
}
// eof

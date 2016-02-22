#include "core_lib.h"

#include <QSize>
#include <qmath.h>
#include <numeric>

namespace core {

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
  return ! (qIsNaN(min) || qIsNaN(max));
}

void Range::set(qreal val) {
  set(val,val);
}

void Range::set(qreal min_, qreal max_) {
  ASSERT((qIsNaN(min_) && qIsNaN(max_)) || min_ <= max_)
  min = min_; max = max_;
}

void Range::safeSet(qreal v1, qreal v2) {
  if (v1 < v2)
    set(v1,v2);
  else
    set(v2,v1);
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

Ranges::Ranges() {
}

bool Ranges::isEmpty() const {
  return ranges.isEmpty();
}

void Ranges::clear() {
  ranges.clear();
}

bool Ranges::add(Range const& range) {
  ranges_t newRanges;

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
  ranges_t newRanges;

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

ImageCut::ImageCut(uint top_, uint bottom_, uint left_, uint right_)
: top(top_), bottom(bottom_), left(left_), right(right_) {
}

bool ImageCut::operator==(ImageCut const& that) {
  return top==that.top && bottom==that.bottom && left==that.left && right==that.right;
}

uint ImageCut::getWidth(QSize const& fullSize) const {
  return fullSize.width() - left - right;
}

uint ImageCut::getHeight(QSize const& fullSize) const {
  return fullSize.height() - top - bottom;
}

uint ImageCut::getCount(QSize const& fullSize) const {
  return getWidth(fullSize) * getHeight(fullSize);
}

qreal deg_rad(qreal rad) {
  return rad * (180 / M_PI);
}

qreal rad_deg(qreal deg) {
  return deg * (M_PI / 180);
}

void Borders::invalidate() {
  gamma.invalidate();
  tth_regular.invalidate();
  tth_gamma0.invalidate();
}

bool Borders::isValid() const {
  return gamma.isValid() && tth_regular.isValid() && tth_gamma0.isValid();
}

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

}

// eof

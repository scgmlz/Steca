#include "core_lib.h"

#include <math.h>

namespace core {

Interval::Interval() {
  clear();
}

Interval::Interval(qreal val) {
  set(val);
}

Interval::Interval(qreal min, qreal max) {
  set(min,max);
}

void Interval::clear() {
  set(qSNaN());
}

bool Interval::isClear() const {
  return qIsNaN(min) || qIsNaN(max);
}

void Interval::set(qreal val) {
  set(val,val);
}

void Interval::set(qreal min_, qreal max_) {
  ASSERT(qIsNaN(min_) && qIsNaN(max_) || min_ <= max_)
  min = min_; max = max_;
}

void Interval::safeSet(qreal v1, qreal v2) {
  if (v1 < v2)
    set(v1,v2);
  else
    set(v2,v1);
}

void Interval::extend(qreal val) {
  min = qIsNaN(min) ? val : qMin(min,val);
  max = qIsNaN(max) ? val : qMax(max,val);
}

void Interval::extend(Interval const& that) {
  extend(that.min); extend(that.max);
}

bool Interval::contains(qreal val) const {
  return min <= val && val <= max;
}

qreal deg_rad(qreal rad) {
  return rad * (180 / M_PI);
}

qreal rad_deg(qreal deg) {
  return deg * (M_PI / 180);
}

}

// eof

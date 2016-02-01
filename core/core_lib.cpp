#include "core_lib.h"

#include <math.h>

namespace core {

Interval::Interval(qreal val) {
  set(val);
}

Interval::Interval(qreal low, qreal high) {
  set(low,high);
}

void Interval::set(qreal val) {
  set(val,val);
}

void Interval::set(qreal low_, qreal high_) {
  ASSERT(low_ <= high_)
  min = low_; max = high_;
}

void Interval::safeSet(qreal v1, qreal v2) {
  if (v1 < v2)
    set(v1,v2);
  else
    set(v2,v1);
}

void Interval::extend(qreal val) {
  min = qMin(min,val); max = qMax(max,val);
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

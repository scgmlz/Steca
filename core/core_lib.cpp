#include "core_lib.h"

#include <QSize>
#include <math.h>

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

void Range::invalidate() {
  set(qQNaN());
}

bool Range::isInvalid() const {
  return qIsNaN(min) || qIsNaN(max);
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

void Range::extend(qreal val) {
  min = qIsNaN(min) ? val : qMin(min,val);
  max = qIsNaN(max) ? val : qMax(max,val);
}

void Range::extend(Range const& that) {
  extend(that.min); extend(that.max);
}

bool Range::contains(qreal val) const {
  return min <= val && val <= max;
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

}

// eof

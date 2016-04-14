// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_curve.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_type_curve.h"
#include "core_fit_functions.h"
#include "core_lens.h"
#include "types/core_type_geometry.h"
#include <qmath.h>

namespace core {
//------------------------------------------------------------------------------

Curve::Curve() {
}

void Curve::clear() {
  xs_.clear(); ys_.clear();
  rgeX_.invalidate();
  rgeY_.invalidate();
}

bool Curve::isEmpty() const {
  return xs_.isEmpty();
}

uint Curve::count() const {
  ASSERT(xs_.count() == ys_.count())
  return xs_.count();
}

bool Curve::isOrdered() const {
  return std::is_sorted(xs_.cbegin(), xs_.cend());
}

void Curve::append(qreal x, qreal y) {
  xs_.append(x);
  ys_.append(y);
  rgeX_.extendBy(x);
  rgeY_.extendBy(y);
}

Curve Curve::intersect(rcRange range) const {
  Curve res;

  ASSERT(isOrdered())

  uint xi = 0, cnt = count();
  auto minX = range.min, maxX = range.max;
  while (xi<cnt && xs_[xi] < minX)
    ++xi;
  while (xi<cnt && xs_[xi] <= maxX) {
    res.append(xs_[xi],ys_[xi]);
    ++xi;
  }

  return res;
}

Curve Curve::intersect(rcRanges ranges) const {
  Curve res;

  // collect points that are in ranges
  // it works because both curve points and ranges are ordered and ranges are non-overlapping
  ASSERT(isOrdered())

  uint xi = 0, cnt = count();
  for_i (ranges.count()) {
    rcRange range = ranges.at(i);
    auto minX = range.min, maxX = range.max;
    while (xi<cnt && xs_[xi] < minX)
      ++xi;
    while (xi<cnt && xs_[xi] <= maxX) {
      res.append(xs_[xi],ys_[xi]);
      ++xi;
    }
  }

  return res;
}

Curve Curve::subtract(fit::Function const& f) const {
  Curve res;

  for_i (xs_.count())
    res.append(xs_[i], ys_[i] - f.y(xs_[i]));

  return res;
}

Curve Curve::smooth3() const {
  Curve res;

  for_i (count()-2)
    res.append(xs_[i+1], (ys_[i] + ys_[i+1] + ys_[i+2]) / 3.);

  return res;
}

uint Curve::maxYindex() const {
  if (isEmpty()) return 0;

  auto yMax = ys_[0]; uint index = 0;

  for_i (count()) {
    auto y = ys_[i];
    if (y > yMax) {
      yMax = y; index = i;
    }
  }

  return index;
}

//------------------------------------------------------------------------------
}
// eof

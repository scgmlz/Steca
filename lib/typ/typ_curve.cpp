/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "typ_curve.h"
#include "def/def_alg.h"

namespace typ {
//------------------------------------------------------------------------------

Curve::Curve() {
}

void Curve::clear() {
  xs_.clear();
  ys_.clear();
  rgeX_.invalidate();
  rgeY_.invalidate();
}

bool Curve::isEmpty() const {
  return xs_.isEmpty();
}

uint Curve::count() const {
  ENSURE(xs_.count() == ys_.count())
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

Curve Curve::intersect(Range::rc range) const {
  Curve res;

  if (!range.isEmpty()) {
    ENSURE(isOrdered())

    uint xi = 0, cnt = count();
    auto minX = range.min, maxX = range.max;
    while (xi < cnt && xs_.at(xi) < minX)
      ++xi;
    while (xi < cnt && xs_.at(xi) <= maxX) {
      res.append(xs_.at(xi), ys_.at(xi));
      ++xi;
    }
  }

  return res;
}

Curve Curve::intersect(Ranges::rc ranges) const {
  Curve res;

  // collect points that are in ranges
  // it works because both curve points and ranges are ordered and ranges are
  // non-overlapping
  ENSURE(isOrdered())

  uint xi = 0, cnt = count();
  for_i (ranges.count()) {
    auto& range = ranges.at(i);
    auto  minX = range.min, maxX = range.max;
    while (xi < cnt && xs_.at(xi) < minX)
      ++xi;
    while (xi < cnt && xs_.at(xi) <= maxX) {
      res.append(xs_.at(xi), ys_.at(xi));
      ++xi;
    }
  }

  return res;
}

void Curve::subtract(Function::rc f) {
  for_i (count())
    ys_[i] -= f.y(xs_.at(i));
}

Curve Curve::mul(qreal factor) const {
  Curve res;

  for_i (count())
    res.append(xs_.at(i), ys_.at(i) * factor);

  return res;
}

Curve Curve::smooth3() const {
  Curve res;

  for_i (count() - 2)
    res.append(xs_.at(i+1), (ys_.at(i) + ys_.at(i+1) + ys_.at(i+2)) / 3.);

  return res;
}

uint Curve::maxYindex() const {
  if (isEmpty())
    return 0;

  auto yMax  = ys_.first();
  uint index = 0;

  for_i (count()) {
    auto y = ys_.at(i);
    if (y > yMax) {
      yMax  = y;
      index = i;
    }
  }

  return index;
}

qreal Curve::sumY() const {
  qreal sum = 0;
  for_i (count())
    sum += ys_.at(i);
  return sum;
}

//------------------------------------------------------------------------------
}
// eof

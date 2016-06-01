// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_curve.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "core_type_curve.h"
#include "core_fit_functions.h"
#include "core_lens.h"
#include "types/core_type_geometry.h"
#include <qmath.h>

namespace core {
//------------------------------------------------------------------------------

Curve::Curve() {}

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

Curve Curve::intersect(rcRange range) const {
  Curve res;

  if (!range.isEmpty()) {
    ENSURE(isOrdered())

    uint xi = 0, cnt = count();
    auto minX = range.min, maxX = range.max;
    while (xi < cnt && xs_[xi] < minX)
      ++xi;
    while (xi < cnt && xs_[xi] <= maxX) {
      res.append(xs_[xi], ys_[xi]);
      ++xi;
    }
  }

  return res;
}

Curve Curve::intersect(rcRanges ranges) const {
  Curve res;

  // collect points that are in ranges
  // it works because both curve points and ranges are ordered and ranges are
  // non-overlapping
  ENSURE(isOrdered())

  uint xi = 0, cnt = count();
  for_i (ranges.count()) {
    rcRange range = ranges.at(i);
    auto    minX = range.min, maxX = range.max;
    while (xi < cnt && xs_[xi] < minX)
      ++xi;
    while (xi < cnt && xs_[xi] <= maxX) {
      res.append(xs_[xi], ys_[xi]);
      ++xi;
    }
  }

  return res;
}

void Curve::subtract(fit::Function const& f) {
  for_i (count())
    ys_[i] -= f.y(xs_.at(i));
}

Curve Curve::add(rcCurve that) const {
  rcCurve c1 = *this, c2 = that;
  uint iEnd1 = c1.count(), iEnd2 = c2.count();
  uint i1 = 0, i2 = 0;

  Curve res;

  for(;;) {
    if (i1 >= iEnd1) {
      // have only that, append its rest
      for (; i2 < iEnd2; ++i2)
        res.append(c2.x(i2), c2.y(i2));
      break;
    } else if (i2 >= iEnd2) {
      // have only this, append its rest
      for (; i1 < iEnd1; ++i1)
        res.append(c1.x(i1), c1.y(i1));
      break;
    } else {
      // have both, append one
      qreal x1 = c1.x(i1), x2 = c2.x(i2);
      if (x1 < x2) {
        res.append(x1,c1.y(i1));
        ++i1;
      } else if (x2 < x1) {
        res.append(x2,c2.y(i2));
        ++i2;
      } else {
        res.append(x1, (c1.y(i1) + c2.y(i2)) / 2);
        ++i1; ++i2;
      }
    }
  }

  return res;
}

Curve Curve::addSimple(rcCurve that) const {
  Curve const *curve1 = this, *curve2 = &that;
  uint         count1 = curve1->count(), count2 = curve2->count();

  // the longer one comes second
  if (count1 > count2) {
    qSwap(curve1, curve2);
    qSwap(count1, count2);
  }

  Curve res;

  // the shorter part - both curves
  for (uint i = 0, iEnd = count1; i < iEnd; ++i)
    res.append(curve2->x(i), curve1->y(i) + curve2->y(i));

  // the remainder of the longer curve
  for (uint i = count1, iEnd = count2; i < iEnd; ++i)
    res.append(curve2->x(i), curve2->y(i));

  return res;
}

Curve Curve::mul(qreal factor) const {
  Curve res;

  for_i (count())
    res.append(xs_[i], ys_[i] * factor);

  return res;
}

Curve Curve::smooth3() const {
  Curve res;

  for_i (count() - 2)
    res.append(xs_[i + 1], (ys_[i] + ys_[i + 1] + ys_[i + 2]) / 3.);

  return res;
}

uint Curve::maxYindex() const {
  if (isEmpty()) return 0;

  auto yMax  = ys_[0];
  uint index = 0;

  for_i (count()) {
    auto y = ys_[i];
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
    sum += ys_[i];
  return sum;
}

//------------------------------------------------------------------------------
}
// eof

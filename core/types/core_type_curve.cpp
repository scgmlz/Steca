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
  _xs.clear(); _ys.clear();
  _rgeX.invalidate();
  _rgeY.invalidate();
}

bool Curve::isEmpty() const {
  return _xs.isEmpty();
}

uint Curve::count() const {
  ASSERT(_xs.count() == _ys.count())
  return _xs.count();
}

bool Curve::isOrdered() const {
  return std::is_sorted(_xs.cbegin(), _xs.cend());
}

void Curve::append(qreal x, qreal y) {
  _xs.append(x);
  _ys.append(y);
  _rgeX.extendBy(x);
  _rgeY.extendBy(y);
}

Curve Curve::intersect(rcRange range) const {
  Curve res;

  ASSERT(isOrdered())

  uint xi = 0, cnt = count();
  auto minX = range.min, maxX = range.max;
  while (xi<cnt && _xs[xi] < minX)
    ++xi;
  while (xi<cnt && _xs[xi] <= maxX) {
    res.append(_xs[xi],_ys[xi]);
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
    while (xi<cnt && _xs[xi] < minX)
      ++xi;
    while (xi<cnt && _xs[xi] <= maxX) {
      res.append(_xs[xi],_ys[xi]);
      ++xi;
    }
  }

  return res;
}

Curve Curve::subtract(fit::Function const& f) const {
  Curve res;

  for_i (_xs.count())
    res.append(_xs[i], _ys[i] - f.y(_xs[i]));

  return res;
}

Curve Curve::smooth3() const {
  Curve res;

  for_i (count()-2)
    res.append(_xs[i+1], (_ys[i] + _ys[i+1] + _ys[i+2]) / 3.);

  return res;
}

uint Curve::maxYindex() const {
  if (isEmpty()) return 0;

  auto yMax = _ys[0]; uint index = 0;

  for_i (count()) {
    auto y = _ys[i];
    if (y > yMax) {
      yMax = y; index = i;
    }
  }

  return index;
}

//------------------------------------------------------------------------------
}
// eof

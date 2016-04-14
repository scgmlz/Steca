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
#include "types/core_type_angles.h"
#include <QtMath>

namespace core {
//------------------------------------------------------------------------------

Curve::Curve() {
}

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
  xRange.extendBy(x);
  yRange.extendBy(y);
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

Curve Curve::subtract(fit::Function const& f) const {
  Curve res;

  for_i (xs.count())
    res.append(xs[i], ys[i] - f.y(xs[i]));

  return res;
}

Curve Curve::smooth3() const {
  Curve res;

  for_i (count()-2)
    res.append(xs[i+1], (ys[i] + ys[i+1] + ys[i+2]) / 3.);

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

Curve makeCurve(shp_LensSystem lenses,
    Range const& gammaRange, Range const& tthRange) {

  Curve res;

  auto size   = lenses->getSize();
  int width = size.width(), height = size.height();

  qreal const deltaTTH = tthRange.width() / width;

  qreal_vec intens_vec(width);
  uint_vec  counts_vec(width, 0);

  for_int (iy, height) {
    for_int (ix, width) {
      // TODO angles can be arranged for a single loop for_i (pixTotal)
      // [last in commit 98413db71cd38ebaa54b6337a6c6e670483912ef]
      auto angles = lenses->getAngles(ix, iy);
      if (!gammaRange.contains(angles.gamma)) continue;

      int bin = (angles.tth == tthRange.max)
                ? width - 1
                : qFloor((angles.tth - tthRange.min) / deltaTTH);

      if (bin < 0 || width <= bin) {
//        TR("TTH bin outside cut?")
        continue; // outside of the cut
      }

      const auto in = lenses->getIntensity(ix, iy);
      if (!qIsNaN(in)) {
        intens_vec[bin] += in;
        counts_vec[bin] += 1;
      }
    }
  }

  for_i (width) {
    auto in  = intens_vec[i];
    auto cnt = counts_vec[i];
    if (cnt > 0) in /= cnt;
    res.append(tthRange.min + deltaTTH * i, in);
  }

  return res;
}

//------------------------------------------------------------------------------
}
// eof

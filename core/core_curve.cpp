#include "core_curve.h"

#include "core_lens.h"

#include <qmath.h>

namespace core {
//------------------------------------------------------------------------------

Curve::Curve() = default;

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
  xRange.extend(x);
  yRange.extend(y);
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

Curve makeCurve(shp_LensSystem const lenses,
                Range const& gammaRange,
                Range const& tthRange) {
  Curve c;

  const auto size     = lenses->getSize();
  const uint width    = size.width();
  const uint height   = size.height();

  const qreal deltaTTH = tthRange.width() / width;

  reals_t intens_vec(width);
  uints_t counts_vec(width,0);

  for(uint iy = 0; iy < height; ++iy) {
    for(uint ix = 0; ix < width; ++ix) {

      // TODO angles can be arranged for a single loop for_i (pixTotal)
      // [last in commit 98413db71cd38ebaa54b6337a6c6e670483912ef]
      const auto angles = lenses->getAngles(ix, iy);
      if (!gammaRange.contains(angles.gamma)) continue;

      const int bin = (angles.tth == tthRange.max)
        ? width - 1 : qFloor((angles.tth - tthRange.min) / deltaTTH);
      if (bin < 0 || static_cast<int> (width) <= bin) {
          TR("TTH bin outside cut?")
          continue;  // outside of the cut
      }

      const auto in = lenses->getIntensity(ix, iy);
      if (!qIsNaN(in)) {
        intens_vec[bin] += in;
        counts_vec[bin] += 1;
      }
    }
  }

  for_i (width) {
    auto in = intens_vec[i];
    const auto cnt = counts_vec[i];
    if (cnt > 0) in /= cnt;
    c.append(tthRange.min + deltaTTH * i, in);
  }

  return c;
}

//------------------------------------------------------------------------------
}

// eof

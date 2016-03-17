#ifndef CORE_CURVE_H
#define CORE_CURVE_H

#include "core_fit_functions.h"

namespace core {
//------------------------------------------------------------------------------
/// A set of datapoints.

class Curve final {
public:
  Curve();

  void clear();

  bool isEmpty() const;
  uint count()   const;
  bool isOrdered() const;

  void append(qreal x, qreal y);

  reals_t const& getXs() const { return xs; }
  reals_t &getXs() { return xs; }
  reals_t const& getYs() const { return ys; }
  reals_t &getYs() { return ys; }

  qreal x(uint i) const { return xs[i]; }
  qreal y(uint i) const { return ys[i]; }

  Range const& getXRange() const { return xRange; }
  Range const& getYRange() const { return yRange; }

  Curve intersect(Range const&)  const;
  Curve intersect(Ranges const&) const;

  void subtractFunction(fit::Function const& f);

  Curve smooth()    const;
  uint  maxYindex() const;

private:
  reals_t xs, ys;
  core::Range xRange, yRange;
};

using Curves = QVector<Curve>;

Curve makeCurve(shp_LensSystem const lenses,
                Range const& gammaRange,
                Range const& tthRange);

}

#endif
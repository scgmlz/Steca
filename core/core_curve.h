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

  qreal_vec const& getXs() const { return xs; }
  qreal_vec const& getYs() const { return ys; }

  qreal x(uint i) const { return xs[i]; }
  qreal y(uint i) const { return ys[i]; }

  Range const& getXRange() const { return xRange; }
  Range const& getYRange() const { return yRange; }

  Curve intersect(Range const&)  const;
  Curve intersect(Ranges const&) const;

  /// Apply unary function to each x.
  void subtractFunction(fit::Function const& f);

  Curve smooth()    const;
  uint  maxYindex() const;

private:
  qreal_vec xs, ys;
  core::Range xRange, yRange;
};

using Curves = QVector<Curve>;

//------------------------------------------------------------------------------

Curve makeCurve(shp_LensSystem const lenses,
                Range const& gammaRange,
                Range const& tthRange);

//------------------------------------------------------------------------------
}

#endif

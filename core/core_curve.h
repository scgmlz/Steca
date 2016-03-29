// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_curve.h
//! @brief     The x-y curve class
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_CURVE_H
#define CORE_CURVE_H

#include "core_types.h"

namespace core {

namespace fit {
class Function;
}

//------------------------------------------------------------------------------
/// A set of datapoints.

class Curve {
public:
  Curve();

  void clear();

  bool isEmpty()    const;
  uint count()      const;
  bool isOrdered()  const;

  void append(qreal x, qreal y);

  qreal_vec const& getXs()  const { return xs; }
  qreal_vec const& getYs()  const { return ys; }

  qreal x(uint i)           const { return xs[i]; }
  qreal y(uint i)           const { return ys[i]; }

  Range const& XRange()     const { return xRange; }
  Range const& YRange()     const { return yRange; }

  Curve intersect(Range const&)   const;
  Curve intersect(Ranges const&)  const;

  Curve subtract(fit::Function const&)  const;
  Curve smooth3()                       const;  ///< moving average, 3 points
  uint  maxYindex()                     const;  ///< the index of the maximum y

private:
  qreal_vec xs, ys;
  core::Range xRange, yRange;
};

typedef QVector<Curve> curve_vec;

//------------------------------------------------------------------------------

Curve makeCurve(shp_LensSystem, Range const& gammaRange, Range const& tthRange);

//------------------------------------------------------------------------------
}
#endif // CORE_CURVE_H

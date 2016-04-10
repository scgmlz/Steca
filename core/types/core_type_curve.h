// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_curve.h
//! @brief     An x-y curve
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPE_CURVE_H
#define CORE_TYPE_CURVE_H

#include "core_defs.h"
#include "core_types_fwd.h"
#include "types/core_type_range.h"

namespace core {
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

  qreal_vec const& xs()  const { return _xs; }
  qreal_vec const& ys()  const { return _ys; }

  qreal x(uint i)        const { return _xs[i]; }
  qreal y(uint i)        const { return _ys[i]; }

  rcRange rgeX()         const { return _rgeX; }
  rcRange rgeY()         const { return _rgeY; }

  Curve intersect(rcRange)   const;
  Curve intersect(rcRanges)  const;

  Curve subtract(fit::Function const&)  const;

  Curve smooth3()   const;  ///< moving average, 3 points
  uint  maxYindex() const;  ///< the index of the maximum y

private:
  qreal_vec _xs, _ys;
  core::Range _rgeX, _rgeY;
};

typedef QVector<Curve> curve_vec;

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_CURVE_H

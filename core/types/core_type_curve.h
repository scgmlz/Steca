// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
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

  // access to data vectors - required by QCP
  qreal_vec const& xs()  const { return xs_; }
  qreal_vec const& ys()  const { return ys_; }

  // prefer this access instead of xs(), ys()
  qreal x(uint i)        const { return xs_.at(i); }
  qreal y(uint i)        const { return ys_.at(i); }

  rcRange rgeX()         const { return rgeX_; }
  rcRange rgeY()         const { return rgeY_; }

  Curve intersect(rcRange)   const;
  Curve intersect(rcRanges)  const;

  Curve subtract(fit::Function const&)  const;
  Curve add(rcCurve) const;
  Curve mul(qreal)  const;

  Curve smooth3()   const;  ///< moving average, 3 points
  uint  maxYindex() const;  ///< the index of the maximum y

private:
  qreal_vec xs_, ys_;
  core::Range rgeX_, rgeY_;
};

typedef QVector<Curve> curve_vec;

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_CURVE_H

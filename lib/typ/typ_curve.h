// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      typ_curve.h
//! @brief     An x-y curve
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_CURVE_H
#define TYP_CURVE_H

#include "def/def_macros.h"
#include "typ/typ_fun.h"
#include "typ/typ_range.h"
#include "typ/typ_curve.h"
#include "typ/typ_vec.h"

namespace typ {
//------------------------------------------------------------------------------
// A set of x-y datapoints.

class Curve {
  CLS(Curve)
public:
  Curve();

  void clear();

  bool isEmpty()    const;
  uint count()      const;
  bool isOrdered()  const;

  void append(qreal x, qreal y);

  // access to underlying data vectors
  qreal_vec::rc xs() const { return xs_; }
  qreal_vec::rc ys() const { return ys_; }

  qreal x(uint i)    const { return xs_.at(i); }
  qreal y(uint i)    const { return ys_.at(i); }

  Range::rc rgeX()   const { return rgeX_; }
  Range::rc rgeY()   const { return rgeY_; }

  Curve intersect(Range::rc)   const;
  Curve intersect(Ranges::rc)  const;

  void  subtract(Function::rc);

  Curve add(rc)       const; // taking values of x into account
  Curve addSimple(rc) const; // just point-by-point, without regard to x
  Curve mul(qreal)    const;

  Curve smooth3()    const;  // moving average, 3 points
  uint  maxYindex()  const;  // the index of the maximum y value

  qreal sumY()       const;

private:
  qreal_vec xs_, ys_;
  Range     rgeX_, rgeY_;
};

typedef vec<Curve> curve_vec;

//------------------------------------------------------------------------------
}
#endif // TYP_CURVE_H

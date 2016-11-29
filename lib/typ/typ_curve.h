/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

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

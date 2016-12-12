/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
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

#ifndef TYP_MATRIX_H
#define TYP_MATRIX_H

#include "../def/def_macros.h"

namespace typ {
//------------------------------------------------------------------------------

struct vector3d;

struct vector3f {
  float _0,_1, _2;

  vector3f(float,float,float);
  vector3f(vector3d const&);
};

struct vector3d {
  qreal _0,_1, _2;

  vector3d(qreal, qreal, qreal);
  vector3d(vector3f const&);
};

struct matrix3d {
  qreal _00, _01, _02,
        _10, _11, _12,
        _20, _21, _22;

  matrix3d(qreal, qreal, qreal,
           qreal, qreal, qreal,
           qreal, qreal, qreal);

  void     transpose();
  matrix3d transposed() const;

  matrix3d operator *(matrix3d const&) const;
  vector3d operator *(vector3d const&) const;

  bool operator ==(matrix3d const&) const;

  // factories
  static matrix3d rotationCWx(qreal angle);
  static matrix3d rotationCWz(qreal angle);
  static matrix3d rotationCCWz(qreal angle);
};

//------------------------------------------------------------------------------
}
#endif // TYP_MATRIX_H

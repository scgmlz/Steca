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

#ifndef TYP_MATRIX_H
#define TYP_MATRIX_H

#include "def/def_macros.h"

namespace typ {
//------------------------------------------------------------------------------

struct vec3r;

struct vec3f {
  typedef vec3f const& rc;

  float _0,_1, _2;

  vec3f(float,float,float);
  vec3f(vec3r const&);

  bool operator==(vec3f::rc) const;
};

struct vec3r {
  typedef vec3r const& rc;

  qreal _0,_1, _2;

  vec3r(qreal, qreal, qreal);
  vec3r(vec3f::rc);

  bool operator==(vec3r::rc) const;
};

struct mat3r {
  typedef mat3r const& rc;

  qreal _00, _01, _02,
        _10, _11, _12,
        _20, _21, _22;

  mat3r(qreal, qreal, qreal,
        qreal, qreal, qreal,
        qreal, qreal, qreal);

  bool  operator==(mat3r::rc) const;

  void  transpose();
  mat3r transposed() const;

  mat3r operator*(mat3r::rc) const;
  vec3r operator*(vec3r::rc) const;

  // factories
  static mat3r rotationCWx(qreal angle);
  static mat3r rotationCWz(qreal angle);
  static mat3r rotationCCWz(qreal angle);
};

//------------------------------------------------------------------------------
}
#endif // TYP_MATRIX_H

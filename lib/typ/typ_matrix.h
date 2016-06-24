// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_matrix.h
//! @brief     3D matrix and vector operations
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_MATRIX_H
#define TYP_MATRIX_H

#include "def/def_macros.h"

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

// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/typ/typ_matrix.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_MATRIX_H
#define TYP_MATRIX_H

#include "def/def_macros.h"

namespace typ {

struct vec3r;

struct vec3f {
  typedef vec3f const& rc;

  float _0, _1, _2;

  vec3f(float, float, float);
  vec3f(vec3r const&);

  bool operator==(vec3f::rc) const;
};

struct vec3r {
  typedef vec3r const& rc;

  qreal _0, _1, _2;

  vec3r(qreal, qreal, qreal);
  vec3r(vec3f::rc);

  bool operator==(vec3r::rc) const;
};

struct mat3r {
  typedef mat3r const& rc;

  qreal _00, _01, _02, _10, _11, _12, _20, _21, _22;

  mat3r(qreal, qreal, qreal, qreal, qreal, qreal, qreal, qreal, qreal);

  bool operator==(mat3r::rc) const;

  void  transpose();
  mat3r transposed() const;

  mat3r operator*(mat3r::rc) const;
  vec3r operator*(vec3r::rc) const;

  // factories
  static mat3r rotationCWx(qreal angle);
  static mat3r rotationCWz(qreal angle);
  static mat3r rotationCCWz(qreal angle);
};
}
#endif  // TYP_MATRIX_H

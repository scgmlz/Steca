// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_matrix.h
//! @brief     3D matrix and vector operations
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPE_MATRIX_H
#define CORE_TYPE_MATRIX_H

#include "core_defs.h"
#include "core_types_fwd.h"

namespace core {
//------------------------------------------------------------------------------

struct vector3d {
  qreal _0, _1, _2;

  vector3d(qreal,qreal,qreal);
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
#endif // CORE_TYPE_MATRIX_H

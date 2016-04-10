// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_matrix.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_type_matrix.h"
#include <qmath.h>

namespace core {
//------------------------------------------------------------------------------

vector3d::vector3d(qreal _0_, qreal _1_, qreal _2_) {
  _0 = _0_; _1 = _1_; _2 = _2_;
}

matrix3d::matrix3d(qreal _00_, qreal _01_, qreal _02_,
                   qreal _10_, qreal _11_, qreal _12_,
                   qreal _20_, qreal _21_, qreal _22_) {
  _00 = _00_; _01 = _01_; _02 = _02_;
  _10 = _10_; _11 = _11_; _12 = _12_;
  _20 = _20_; _21 = _21_; _22 = _22_;
}

void matrix3d::transpose() {
  qSwap(_01,_10);
  qSwap(_02,_20);
  qSwap(_12,_21);
}

matrix3d matrix3d::transposed() const {
  return matrix3d(_00, _10, _20,
                  _01, _11, _21,
                  _02, _12, _22);
}

matrix3d matrix3d::operator *(matrix3d const& n) const {
  return matrix3d(
    _00*n._00 + _01*n._10 + _02*n._20,
    _00*n._01 + _01*n._11 + _02*n._21,
    _00*n._02 + _01*n._12 + _02*n._22,

    _10*n._00 + _11*n._10 + _12*n._20,
    _10*n._01 + _11*n._11 + _12*n._21,
    _10*n._02 + _11*n._12 + _12*n._22,

    _20*n._00 + _21*n._10 + _22*n._20,
    _20*n._01 + _21*n._11 + _22*n._21,
    _20*n._02 + _21*n._12 + _22*n._22
  );
}

vector3d matrix3d::operator *(vector3d const& v) const {
  return vector3d(
    _00*v._0 + _01*v._1 + _02*v._2,
    _10*v._0 + _11*v._1 + _12*v._2,
    _20*v._0 + _21*v._1 + _22*v._2
  );
}

bool matrix3d::operator ==(matrix3d const& n) const {
  return
    _00==n._00 && _01==n._01 && _02==n._02 &&
    _10==n._10 && _11==n._11 && _12==n._12 &&
      _20==n._20 && _21==n._21 && _22==n._22;
}

matrix3d matrix3d::rotationCWx(qreal angle) {
  return matrix3d(1, 0,           0,
                  0, cos(angle), -sin(angle),
                  0, sin(angle),  cos(angle));
}

matrix3d matrix3d::rotationCWz(qreal angle) {
  return matrix3d(cos(angle), -sin(angle), 0,
                  sin(angle),  cos(angle), 0,
                  0,           0,          1);
}

matrix3d matrix3d::rotationCCWz(qreal angle) {
  return rotationCWz(angle).transposed();
}

//------------------------------------------------------------------------------
}
// eof

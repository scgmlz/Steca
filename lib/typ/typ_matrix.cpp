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

#include "typ_matrix.h"

#include <qmath.h>

#include "test/tests.h"

namespace typ {
//------------------------------------------------------------------------------

//#ifdef TESTS
//template <typename T, typename O>
//bool e(T const& t, O const& o) {
//  return qAbs(qreal(t) - qreal(o)) < .00001;
//}

//template <typename T, typename O>
//bool VEC3_EQ(T const& t, O const& o) {
//  return e(t._0, o._0) && e(t._1, o._1) && e(t._2, o._2);
//}
//#endif

vec3f::vec3f(float _0_, float _1_, float _2_) {
  _0 = _0_; _1 = _1_; _2 = _2_;
}

bool vec3f::operator==(rc that) const {
  return _0 == that._0 && _1 == that._1 && _2 == that._2;
}

vec3f::vec3f(vec3r::rc v)
: vec3f(float(v._0), float(v._1), float(v._2)) {
}

vec3r::vec3r(qreal _0_, qreal _1_, qreal _2_) {
  _0 = _0_; _1 = _1_; _2 = _2_;
}

vec3r::vec3r(vec3f::rc v)
  : vec3r(qreal(v._0), qreal(v._1), qreal(v._2)) {
}

bool vec3r::operator==(rc that) const {
  return _0 == that._0 && _1 == that._1 && _2 == that._2;
}

TEST("vec3", ({
  vec3f f(1,2,3); vec3r r(1,2,3);
  vec3f fr(r);    vec3r rf(f);

  CHECK_EQ(f, r);
  CHECK_EQ(f, fr);
  CHECK_EQ(f, rf);
  CHECK_EQ(r, fr);
  CHECK_EQ(r, rf);
  CHECK_EQ(fr, rf);
}););

mat3r::mat3r(qreal _00_, qreal _01_, qreal _02_,
                   qreal _10_, qreal _11_, qreal _12_,
                   qreal _20_, qreal _21_, qreal _22_) {
  _00 = _00_; _01 = _01_; _02 = _02_;
  _10 = _10_; _11 = _11_; _12 = _12_;
  _20 = _20_; _21 = _21_; _22 = _22_;
}

bool mat3r::operator==(mat3r::rc n) const {
  return
    _00==n._00 && _01==n._01 && _02==n._02 &&
    _10==n._10 && _11==n._11 && _12==n._12 &&
    _20==n._20 && _21==n._21 && _22==n._22;
}

void mat3r::transpose() {
  qSwap(_01, _10);
  qSwap(_02, _20);
  qSwap(_12, _21);
}

mat3r mat3r::transposed() const {
  return mat3r(_00, _10, _20,
                  _01, _11, _21,
                  _02, _12, _22);
}

mat3r mat3r::operator *(mat3r::rc n) const {
  return mat3r(
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

vec3r mat3r::operator *(vec3r::rc v) const {
  return vec3r(
    _00*v._0 + _01*v._1 + _02*v._2,
    _10*v._0 + _11*v._1 + _12*v._2,
    _20*v._0 + _21*v._1 + _22*v._2
  );
}

TEST("mat3r", ({
  mat3r m1(0,1,2, 3,4,5, 6,7,8), mc(m1);
  mat3r mt(0,3,6, 1,4,7, 2,5,8);

  CHECK_EQ(m1.transposed(), mt);
  mc.transpose();
  CHECK_EQ(mc, mt);

  CHECK_EQ(vec3r(17,62,107), m1 * vec3r(4,5,6));
  CHECK_EQ(mat3r(5,14,23, 14,50,86, 23,86,149), m1*mt);
}););

mat3r mat3r::rotationCWx(qreal angle) {
  return mat3r(1, 0,           0,
               0, cos(angle), -sin(angle),
               0, sin(angle),  cos(angle));
}

mat3r mat3r::rotationCWz(qreal angle) {
  return mat3r(cos(angle), -sin(angle), 0,
               sin(angle),  cos(angle), 0,
               0,           0,          1);
}

mat3r mat3r::rotationCCWz(qreal angle) {
  return rotationCWz(angle).transposed();
}

TEST("mat3r::rotation", ({
  qreal angle = 1;
  auto cwx  = mat3r::rotationCWx(angle);
  auto cwz  = mat3r::rotationCWz(angle);
  auto ccwz = mat3r::rotationCCWz(angle);

  vec3r v(1,2,3);
  CHECK_EQ(v, cwx.transposed()*(cwx*v));
  CHECK_EQ(v, ccwz*(cwz*v));
}););

//------------------------------------------------------------------------------
}
// eof

//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/matrix.cpp
//! @brief     Implements the structs vec3f, vec3r, mat3r
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test001_matrix
//
//  ***********************************************************************************************

#include "core/calc/matrix.h"
#include <qmath.h>

//  ***********************************************************************************************
//! @class vec3f

vec3f::vec3f(float _0_, float _1_, float _2_)
{
    _0 = _0_;
    _1 = _1_;
    _2 = _2_;
}

vec3f::vec3f(const vec3r& v)
    : vec3f(float(v._0), float(v._1), float(v._2))
{}

bool vec3f::operator==(rc that) const
{
    return _0 == that._0 && _1 == that._1 && _2 == that._2;
}

//  ***********************************************************************************************
//! @class vec3r

vec3r::vec3r(double _0_, double _1_, double _2_)
{
    _0 = _0_;
    _1 = _1_;
    _2 = _2_;
}

vec3r::vec3r(const vec3f& v)
    : vec3r(double(v._0), double(v._1), double(v._2))
{}

bool vec3r::operator==(rc that) const
{
    return _0 == that._0 && _1 == that._1 && _2 == that._2;
}

//  ***********************************************************************************************
//! @class mat3r

mat3r::mat3r(double _00_, double _01_, double _02_,
             double _10_, double _11_, double _12_,
             double _20_, double _21_, double _22_)
{
    _00 = _00_;
    _01 = _01_;
    _02 = _02_;
    _10 = _10_;
    _11 = _11_;
    _12 = _12_;
    _20 = _20_;
    _21 = _21_;
    _22 = _22_;
}

bool mat3r::operator==(const mat3r& n) const
{
    return _00 == n._00 && _01 == n._01 && _02 == n._02 && _10 == n._10 && _11 == n._11
        && _12 == n._12 && _20 == n._20 && _21 == n._21 && _22 == n._22;
}

void mat3r::transpose()
{
    qSwap(_01, _10);
    qSwap(_02, _20);
    qSwap(_12, _21);
}

mat3r mat3r::transposed() const
{
    return mat3r(_00, _10, _20, _01, _11, _21, _02, _12, _22);
}

mat3r mat3r::operator*(const mat3r& n) const
{
    return mat3r(
        _00 * n._00 + _01 * n._10 + _02 * n._20, _00 * n._01 + _01 * n._11 + _02 * n._21,
        _00 * n._02 + _01 * n._12 + _02 * n._22,

        _10 * n._00 + _11 * n._10 + _12 * n._20, _10 * n._01 + _11 * n._11 + _12 * n._21,
        _10 * n._02 + _11 * n._12 + _12 * n._22,

        _20 * n._00 + _21 * n._10 + _22 * n._20, _20 * n._01 + _21 * n._11 + _22 * n._21,
        _20 * n._02 + _21 * n._12 + _22 * n._22);
}

vec3r mat3r::operator*(const vec3r& v) const
{
    return vec3r(
        _00 * v._0 + _01 * v._1 + _02 * v._2, _10 * v._0 + _11 * v._1 + _12 * v._2,
        _20 * v._0 + _21 * v._1 + _22 * v._2);
}

mat3r mat3r::rotationCWx(double angle)
{
    return mat3r(1, 0, 0, 0, cos(angle), -sin(angle), 0, sin(angle), cos(angle));
}

mat3r mat3r::rotationCWz(double angle)
{
    return mat3r(cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1);
}

mat3r mat3r::rotationCCWz(double angle)
{
    return rotationCWz(angle).transposed();
}

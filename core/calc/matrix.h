//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/matrix.h
//! @brief     Defines the structs vec3f, vec3r, mat3r
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef MATRIX_H
#define MATRIX_H

struct vec3r;

//! 3D vector with base type float.
struct vec3f {
    typedef const vec3f& rc;

    float _0, _1, _2;

    vec3f(float, float, float);
    vec3f(const vec3r&);

    bool operator==(const vec3f&) const;
};

//! 3D vector with base type double.
struct vec3r {
    typedef const vec3r& rc;

    double _0, _1, _2;

    vec3r(double, double, double);
    vec3r(const vec3f&);

    bool operator==(const vec3r&) const;
};

//! Rotation matrix in 3D, with base type double.
struct mat3r {
    typedef const mat3r& rc;

    double _00, _01, _02, _10, _11, _12, _20, _21, _22;

    mat3r(double, double, double, double, double, double, double, double, double);

    bool operator==(const mat3r&) const;

    void transpose();
    mat3r transposed() const;

    mat3r operator*(const mat3r&) const;
    vec3r operator*(const vec3r&) const;

    // factories
    static mat3r rotationCWx(double angle);
    static mat3r rotationCWz(double angle);
    static mat3r rotationCCWz(double angle);
};

#endif // MATRIX_H

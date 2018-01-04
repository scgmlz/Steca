// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/matrix.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MATRIX_H
#define MATRIX_H

#include "def/macros.h"

struct vec3r;

struct vec3f {
    typedef vec3f const& rc;

    float _0, _1, _2;

    vec3f(float, float, float);
    vec3f(vec3r const&);

    bool operator==(vec3f const&) const;
};

struct vec3r {
    typedef vec3r const& rc;

    qreal _0, _1, _2;

    vec3r(qreal, qreal, qreal);
    vec3r(vec3f const&);

    bool operator==(vec3r const&) const;
};

struct mat3r {
    typedef mat3r const& rc;

    qreal _00, _01, _02, _10, _11, _12, _20, _21, _22;

    mat3r(qreal, qreal, qreal, qreal, qreal, qreal, qreal, qreal, qreal);

    bool operator==(mat3r const&) const;

    void transpose();
    mat3r transposed() const;

    mat3r operator*(mat3r const&) const;
    vec3r operator*(vec3r const&) const;

    // factories
    static mat3r rotationCWx(qreal angle);
    static mat3r rotationCWz(qreal angle);
    static mat3r rotationCCWz(qreal angle);
};

#endif // MATRIX_H

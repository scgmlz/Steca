// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/angles.cpp
//! @brief     Implements  classes deg and rad.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test003_angles
//
// ************************************************************************** //

#include "core/typ/angles.h"
#include <qmath.h>

deg::deg(rad r) {
    val_ = r.toDeg();
}

rad deg::toRad() const {
    return val_ * (M_PI / 180);
}

deg& deg::operator+=(deg const& that) {
    val_ += that.val_;
    return *this;
}

deg& deg::operator*=(qreal fac) {
    val_ *= fac;
    return *this;
}

deg deg::normalized() {
    static qreal const MAX = 360;
    qreal norm = fmod(val_, MAX);
    if (norm < 0)
        norm += MAX;
    return norm;
}

rad::rad(deg d) {
    val_ = d.toRad();
}

deg rad::toDeg() const {
    return val_ * (180 / M_PI);
}

rad& rad::operator+=(rad const& that) {
    val_ += that.val_;
    return *this;
}

rad& rad::operator*=(qreal fac) {
    val_ *= fac;
    return *this;
}

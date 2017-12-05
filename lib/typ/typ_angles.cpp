// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/typ/typ_angles.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ_angles.h"
#include <qmath.h>

#include "test/tests.h"

namespace typ {

deg::deg(rad r) {
    val_ = r.toDeg();
}

TEST("deg(rad)", ({ CHECK_EQ(qreal(deg(rad(M_PI_2))), 90); });)

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

TEST("rad(deg)", ({ CHECK_EQ(qreal(rad(deg(90))), M_PI_2); });)

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
}

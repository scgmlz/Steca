//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/misc/angles.cpp
//! @brief     Implements  classes deg and rad.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test003_angles
//
//  ***********************************************************************************************

#include "core/misc/angles.h"
#include <qmath.h>

deg::deg(rad r)
{
    val_ = r.toDeg();
}

rad deg::toRad() const
{
    return rad(val_ * (M_PI / 180));
}

deg& deg::operator+=(const deg& that)
{
    val_ += that.val_;
    return *this;
}

deg& deg::operator*=(double fac)
{
    val_ *= fac;
    return *this;
}

deg deg::normalized() const
{
    static const deg MAX {360};
    deg norm {fmod(val_, MAX)};
    if (norm < 0)
        norm += MAX;
    return norm;
}

rad::rad(deg d)
{
    val_ = d.toRad();
}

deg rad::toDeg() const
{
    return deg(val_ * (180 / M_PI));
}

rad& rad::operator+=(const rad& that)
{
    val_ += that.val_;
    return *this;
}

rad& rad::operator*=(double fac)
{
    val_ *= fac;
    return *this;
}

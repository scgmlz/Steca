// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/angles.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef ANGLES_H
#define ANGLES_H

#include "core/def/macros.h"

class rad;

//! An angle in degrees
class deg {
public:
    deg(qreal val = 0) : val_(val) {}
    operator qreal() const { return val_; }

    explicit deg(rad);
    rad toRad() const;

    deg& operator+=(deg const&);
    deg& operator*=(qreal);

    deg normalized();

private:
    qreal val_;
};

//! An angle in radians
class rad {
public:
    rad(qreal val = 0) : val_(val) {}
    operator qreal() const { return val_; }

    explicit rad(deg);
    deg toDeg() const;

    rad& operator+=(rad const&);
    rad& operator*=(qreal);

private:
    qreal val_;
};

#endif // ANGLES_H

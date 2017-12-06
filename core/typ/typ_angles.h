// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_angles.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_ANGLES_H
#define TYP_ANGLES_H

#include "def/def_macros.h"

namespace typ {

class rad;

// angles stored in degrees
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

// trigonometry needs radians
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
}
#endif // TYP_ANGLES_H

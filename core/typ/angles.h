// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/angles.h
//! @brief     Defines classes deg and rad.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef ANGLES_H
#define ANGLES_H

#include <QtGlobal>

class rad;

//! An angle in degrees
class deg {
public:
    deg(double val = 0) : val_(val) {}
    operator double() const { return val_; }

    explicit deg(rad);
    rad toRad() const;

    deg& operator+=(const deg&);
    deg& operator*=(double);

    deg normalized();

private:
    double val_;
};

//! An angle in radians
class rad {
public:
    rad(double val = 0) : val_(val) {}
    operator double() const { return val_; }

    explicit rad(deg);
    deg toDeg() const;

    rad& operator+=(const rad&);
    rad& operator*=(double);

private:
    double val_;
};

#endif // ANGLES_H

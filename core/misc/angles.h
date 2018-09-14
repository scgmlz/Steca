//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/misc/angles.h
//! @brief     Defines classes deg and rad.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef ANGLES_H
#define ANGLES_H

class rad;

//! An angle in degrees
class deg {
public:
    deg(double val = 0) : val_(val) {}
    explicit deg(rad);

    deg& operator+=(const deg&);
    deg& operator*=(double);

    operator double() const { return val_; }
    rad toRad() const;
    deg normalized() const;

private:
    double val_;
};

//! An angle in radians
class rad {
public:
    rad(double val = 0) : val_(val) {}
    explicit rad(deg);

    rad& operator+=(const rad&);
    rad& operator*=(double);

    operator double() const { return val_; }
    deg toDeg() const;

private:
    double val_;
};

#endif // ANGLES_H

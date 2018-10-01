//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/size2d.h
//! @brief     Defines and mostly implements size2d
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef SIZE2D_H
#define SIZE2D_H

//! A pair of integers, interpreted as width and height of a rectangle.

struct size2d {
    int w, h;

    size2d() : size2d(0, 0) {}
    size2d(int w_, int h_) : w(w_), h(h_) {}

    bool isEmpty() const { return w==0 && h==0; }

    int count() const { return w * h; }

    friend size2d operator-(const size2d& s1, const size2d& s2); // TODO more explicit fct name

    size2d transposed() const { return size2d(h, w); }

    bool operator==(const size2d& other) const { return w==other.w && h==other.h; }
    bool operator!=(const size2d& other) const { return !(*this==other); }
};

#endif // SIZE2D_H

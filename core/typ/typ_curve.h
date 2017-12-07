// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_curve.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_CURVE_H
#define TYP_CURVE_H

#include "typ/typ_fun.h"

namespace typ {

// A set of x-y datapoints.

class Curve {
    CLASS(Curve)
public:
    Curve();

    void clear();

    bool isEmpty() const;
    uint count() const;
    bool isOrdered() const;

    void append(qreal x, qreal y);

    // access to underlying data vectors
    qreal_vec::rc xs() const { return xs_; }
    qreal_vec::rc ys() const { return ys_; }

    qreal x(uint i) const { return xs_.at(i); }
    qreal y(uint i) const { return ys_.at(i); }

    Range::rc rgeX() const { return rgeX_; }
    Range::rc rgeY() const { return rgeY_; }

    Curve intersect(Range::rc) const;
    Curve intersect(Ranges::rc) const;

    void subtract(Function::rc);

    uint maxYindex() const; // the index of the maximum y value

    qreal sumY() const;

private:
    qreal_vec xs_, ys_;
    Range rgeX_, rgeY_;
};

typedef vec<Curve> curve_vec;
}
#endif // TYP_CURVE_H

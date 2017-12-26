// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/curve.h
//! @brief     Defines class Curve
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CURVE_H
#define CURVE_H

#include "typ/range.h"

namespace typ {

class Function;

//! A set of x-y datapoints

class Curve {
public:
    void clear();

    bool isEmpty() const;
    uint count() const;
    bool isOrdered() const;

    void append(qreal x, qreal y);

    // access to underlying data vectors
    qreal_vec const& xs() const { return xs_; }
    qreal_vec const& ys() const { return ys_; }

    qreal x(uint i) const { return xs_.at(i); }
    qreal y(uint i) const { return ys_.at(i); }

    Range const& rgeX() const { return rgeX_; }
    Range const& rgeY() const { return rgeY_; }

    Curve intersect(Range const&) const;
    Curve intersect(Ranges const&) const;

    void subtract(Function const&);

    uint maqpairindex() const; // the index of the maximum y value

    qreal sumY() const;

private:
    qreal_vec xs_, ys_;
    Range rgeX_, rgeY_;
};

typedef vec<Curve> curve_vec;

} // namespace typ

#endif // CURVE_H

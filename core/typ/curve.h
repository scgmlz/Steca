// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/curve.h
//! @brief     Defines class Curve
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CURVE_H
#define CURVE_H

#include "core/typ/range.h"
#include <functional> // required by some compilers

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

    const Range& rgeX() const { return rgeX_; }
    const Range& rgeY() const { return rgeY_; }

    Curve intersect(const Range&) const;
    Curve intersect(const Ranges&) const;

    void subtract(std::function<qreal(qreal)> const& func);

    uint maqpairindex() const; // the index of the maximum y value

    qreal sumY() const;

private:
    qreal_vec xs_, ys_;
    Range rgeX_, rgeY_;
};

typedef vec<Curve> curve_vec;

#endif // CURVE_H

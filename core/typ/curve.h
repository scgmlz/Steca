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
    int count() const;
    bool isOrdered() const;

    void append(qreal x, qreal y);

    // access to underlying data vectors
    const vec<qreal>& xs() const { return xs_; }
    const vec<qreal>& ys() const { return ys_; }

    qreal x(int i) const { return xs_.at(i); }
    qreal y(int i) const { return ys_.at(i); }

    const Range& rgeX() const { return rgeX_; }
    const Range& rgeY() const { return rgeY_; }

    Curve intersect(const Range&) const;
    Curve intersect(const Ranges&) const;

    void subtract(const std::function<qreal(qreal)>& func);

    int maqpairindex() const; // the index of the maximum y value

    qreal sumY() const;

private:
    vec<qreal> xs_, ys_;
    Range rgeX_, rgeY_;
};

typedef vec<Curve> curve_vec;

#endif // CURVE_H

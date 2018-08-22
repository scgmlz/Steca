//  ***********************************************************************************************
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
//  ***********************************************************************************************

#ifndef CURVE_H
#define CURVE_H

#include "core/typ/range.h"
#include <functional> // required by some compilers

//! A set of x-y datapoints

class Curve {
public:
    void clear();
    void append(double x, double y);

    bool isEmpty() const;
    int size() const;

    // access to underlying data vectors
    const std::vector<double>& xs() const { return xs_; }
    const std::vector<double>& ys() const { return ys_; }

    double x(int i) const { return xs_.at(i); }
    double y(int i) const { return ys_.at(i); }

    const Range& rgeX() const { return rgeX_; }
    const Range& rgeY() const { return rgeY_; }

    Curve intersect(const Range&) const;
    Curve intersect(const Ranges&) const;
    Curve subtract(const std::function<double(double)>& func) const;

    int idxMax() const; //!< the index of the maximum y value

    double sumY() const;

private:
    std::vector<double> xs_, ys_;
    Range rgeX_, rgeY_;
};

#endif // CURVE_H

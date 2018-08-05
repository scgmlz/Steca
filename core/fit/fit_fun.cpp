//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_fun.cpp
//! @brief     Implements classes ParametricFunction, Polynom, PeakFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "fit_fun.h"
#include "core/typ/curve.h"
#include "core/fit/fit_methods.h"
#include "qcr/base/debug.h"

namespace {

//! Computes a low integer power of x.
static double pow_n(double x, int n)
{
    double ret = 1;
    while (n-- > 0)
        ret *= x;
    return ret;
}

} // namespace

//  ***********************************************************************************************
//! @class ParametricFunction

void ParametricFunction::reset()
{
    for (auto& p: parameters_)
        p.reset();
}

// outside the fit routine, functions y(x) are called with parValues==nullptr

double ParametricFunction::parValue(int ip, double const* parValues) const
{
    return parValues ? parValues[ip] : parameters_.at(ip).value();
}

//  ***********************************************************************************************
//! @class Polynom

double Polynom::y(double x, double const* parValues) const
{
    double ret = 0, xPow = 1;
    for (int i=0; i<parameters_.size(); ++i) {
        ret += parValue(i, parValues) * xPow;
           // outside the fit routine, functions y(x) are called with parValues==nullptr;
           // therefore we need 'parValue' to access either 'parValues' or 'parameters_'.
        xPow *= x;
    }
    return ret;
}

double Polynom::dy(double x, int i, double const*) const
{
    return pow_n(x, i);
}

Polynom Polynom::fromFit(int degree, const Curve& curve, const Ranges& ranges)
{
    qDebug() << "polynomial fit";
    ASSERT(curve.count()>0);
    ASSERT(ranges.count()>0);
    Polynom p(degree);
    FitWrapper().execFit(p, curve.intersect(ranges));
    return p;
}

//  ***********************************************************************************************
//! @class PeakFunction

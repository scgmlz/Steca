//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/polynom.cpp
//! @brief     Implements class Polynom
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/fit/polynom.h"
#include "core/typ/curve.h"
#include "core/fit/fit_methods.h"

void Polynom::setY(const double* P, const int nXY, const double* X, double* Y) const
{
    for (int i=0 ; i<nXY; ++i) {
        double ret = 0;
        double xPow = 1;
        for (int ip=0; ip<nPar_; ++ip) {
            ret += P[ip] * xPow;
            xPow *= *(X+i);
        }
        Y[i] = ret;  // <--- return values are set here
    }
}

void Polynom::setDY(const double*, const int nXY, const double* X, double* Jacobian) const
{
    for (int i=0; i<nXY; ++i) {
        double xPow = 1;
        for (int ip=0; ip<nPar_; ++ip) {
            *Jacobian++ = xPow;  // <--- return values are set here
            xPow *= *(X+i);
        }
    }
}

ParametricFunction Polynom::fromFit(int degree, const Curve& curve, const Ranges& ranges)
{
    ParametricFunction F(degree+1, new Polynom(degree));
    FitWrapper().execFit(F, curve.intersect(ranges));
    return F;
}

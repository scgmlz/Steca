//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.cpp
//! @brief     Implements classes ParametricFunction, Polynom, PeakFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/fit/parametric_function.h"
#include "core/typ/curve.h"
#include "qcr/base/debug.h" // ASSERT
#include <cmath>

//  ***********************************************************************************************
//! @class FitParameter

FitParameter::FitParameter(double value, double error)
    : value_(value)
    , error_(error)
{}

void FitParameter::setValue(double value, double error)
{
    value_ = value;
    error_ = error;
}

//! Rounds error_ to prec digits, including leading zeros as given by the rounding of value_.

//! Covered by test002_rounding.

double FitParameter::roundedError(int prec) const
{
    int n = 1+lrintf(floor(log10(std::max(std::abs(value_),std::abs(error_)))));
    double fac = pow(10.,prec-n);
    return round(error_*fac)/fac;
}

//  ***********************************************************************************************
//! @class ParametricFunction

// TODO replace by vectorial access wherever possible,
//      or/and maintain values vector within parameters_.

double ParametricFunction::y(const double x) const
{
    double pars[parameters_.size()];
    for (int ip=0; ip<parameters_.size(); ++ip)
        pars[ip] = parameters_[ip].value();
    double ret;
    f->setY(pars, 1, &x, &ret);
    return ret;
}

//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fitengine/double_with_error.cpp
//! @brief     Implements class DoubleWithError
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/fitengine/double_with_error.h"
#include <qmath.h>

//! Rounds error_ to prec digits, including leading zeros as given by the rounding of value_.

//! Covered by test002_rounding.

DoubleWithError::DoubleWithError(double value, double error)
    : value_{value}
    , error_ {error}
{}

double DoubleWithError::roundedError(int prec) const
{
    int n = 1+lrintf(floor(log10(std::max(std::abs(value_),std::abs(error_)))));
    double fac = pow(10.,prec-n);
    return round(error_*fac)/fac;
}

//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_par.cpp
//! @brief     Implements class FitParameter
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/fit/fit_par.h"
#include "qcr/base/debug.h"
#include <cmath>

//  ***********************************************************************************************
//! @class FitParameter

FitParameter::FitParameter(double value, double error)
    : value_(value)
    , error_(error)
    , range_(Range::infinite())
{}

void FitParameter::setAllowedRange(double min, double max)
{
    range_.set(min, max);
}

const Range& FitParameter::range() const {
    ASSERT (range_.isValid());
    return range_;
}

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

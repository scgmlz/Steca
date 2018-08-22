//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.cpp
//! @brief     Implements classes DoubleWithError, FitOutcome
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
//! @class DoubleWithError

DoubleWithError::DoubleWithError(double value, double error)
    : value_(value)
    , error_(error)
{}

void DoubleWithError::setValue(double value, double error)
{
    value_ = value;
    error_ = error;
}

//! Rounds error_ to prec digits, including leading zeros as given by the rounding of value_.

//! Covered by test002_rounding.

double DoubleWithError::roundedError(int prec) const
{
    int n = 1+lrintf(floor(log10(std::max(std::abs(value_),std::abs(error_)))));
    double fac = pow(10.,prec-n);
    return round(error_*fac)/fac;
}

//  ***********************************************************************************************
//! @class FitOutcome

FitOutcome::FitOutcome(const FitFunction* _f,
                       const std::vector<double>& parValue, const std::vector<double>& parError)
        : f{_f}
        , success_{true}
{
    const int nPar = parValue.size();
    ASSERT(parError.size()==nPar);
    for (int i=0; i<nPar; ++i)
        parameters_.push_back({parValue[i], parError[i]});
}

// TODO replace by vectorial access wherever possible

double FitOutcome::y(const double x) const
{
    ASSERT(success_); // computing y makes only sense after a successful fit
    double pars[parameters_.size()];
    for (int ip=0; ip<parameters_.size(); ++ip)
        pars[ip] = parameters_[ip].value();
    double ret;
    f->setY(pars, 1, &x, &ret);
    return ret;
}

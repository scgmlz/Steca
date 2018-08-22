//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.cpp
//! @brief     Implements classes DoubleWithError, Fitted
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

//! Rounds error_ to prec digits, including leading zeros as given by the rounding of value_.

//! Covered by test002_rounding.

double DoubleWithError::roundedError(int prec) const
{
    int n = 1+lrintf(floor(log10(std::max(std::abs(value),std::abs(error)))));
    double fac = pow(10.,prec-n);
    return round(error*fac)/fac;
}

//  ***********************************************************************************************
//! @class Fitted

Fitted::Fitted(const FitFunction* _f,
               const std::vector<double>& _parVal, const std::vector<double>& _parErr)
        : success{true}
        , f{_f}
        , parVal{_parVal}
        , parErr{_parErr}
{
    ASSERT(parErr.size()==parVal.size());
}

// TODO replace by vectorial access wherever possible

double Fitted::y(const double x) const
{
    ASSERT(success); // computing y makes only sense after a successful fit
    double ret;
    f->setY(parVal.data(), 1, &x, &ret);
    return ret;
}

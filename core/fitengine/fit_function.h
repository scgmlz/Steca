//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fitengine/fit_function.h
//! @brief     Defines the header-only class FitFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FIT_FUNCTION_H
#define FIT_FUNCTION_H

//! Abstract base class for all fit functions.
//! Child classes must provide the function y(x) and its Jacobian.

class FitFunction {
public:
    FitFunction() {}
    virtual ~FitFunction() {}
    FitFunction(const FitFunction&) = delete;
    FitFunction(FitFunction&&) = default;
    virtual void setY(const double* P, const int nXY, const double* X, double* Y) const = 0;
    virtual void setDY(const double* P, const int nXY, const double* X,
                       double* Jacobian) const = 0;
    virtual int nPar() const = 0;
};

#endif // FIT_FUNCTION_H

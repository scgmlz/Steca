//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.h
//! @brief     Defines classes DoubleWithError, FitFunction, Fitted
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PARAMETRIC_FUNCTION_H
#define PARAMETRIC_FUNCTION_H

#include <memory>
#include <vector>

//! One double value with error estimate. Used to hold fitted parameter.

class DoubleWithError {
public:
    double roundedError(int) const;

    const double value;
    const double error;
};


//! Holds instructions how to compute y(x) and its Jacobian. Base for Polynom and PeakFunction.

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

class Curve;


//! The outcome of a fit: a function, some fitted parameters, and a success flag.

class Fitted {
public:
    Fitted() {}                                 //!< When fit has failed.
    Fitted(const FitFunction* _f,
           const std::vector<double>& _parVal,
           const std::vector<double>& _parErr); //!< To hold outcome of successful fit
    ~Fitted() { delete f; }
    Fitted(const Fitted&) = delete;
    Fitted(Fitted&&) = default;

    double y(const double x) const;

    const bool success {false};
    const FitFunction* f {nullptr};
    const std::vector<double> parVal;
    const std::vector<double> parErr;

private:
};

#endif // PARAMETRIC_FUNCTION_H

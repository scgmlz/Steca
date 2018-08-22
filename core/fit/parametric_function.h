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
    DoubleWithError(double value=0, double error=0);

    void setValue(double value, double error); // TODO rm ?

    double value() const { return value_; }
    double error() const { return error_; }
    double roundedError(int) const;

private:
    double value_, error_;
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
    Fitted(const FitFunction* _f,
               const std::vector<double>& parValue, const std::vector<double>& parError);
    ~Fitted() { delete f; }
    Fitted(const Fitted&) = delete;
    Fitted(Fitted&&) = default;

    static Fitted Failure() { return Fitted(); }

    double y(const double x) const;
    bool success() const { return success_; }

    // TODO: in getCenter etc, do we really need to pass _all_ parameters?
    // const DoubleWithError& parameterAt(int ip) const { return parameters_[ip]; }
    const std::vector<DoubleWithError>& parameters() const { return parameters_; };

    const FitFunction* f {nullptr};

private:
    Fitted() {}
    std::vector<DoubleWithError> parameters_; // would be const if we knew how to initialize
    const bool success_ {false};
};

#endif // PARAMETRIC_FUNCTION_H

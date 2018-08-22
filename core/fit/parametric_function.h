//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.h
//! @brief     Defines classes DoubleWithError, FitFunction, ParametricFunction
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
    virtual ~FitFunction() {}
    virtual void setY(const double* P, const int nXY, const double* X, double* Y) const = 0;
    virtual void setDY(const double* P, const int nXY, const double* X,
                       double* Jacobian) const = 0;
    virtual int nPar() const = 0;
};

class Curve;
class RawOutcome;


//! A function with fitted parameters.

class ParametricFunction {
public:
    ParametricFunction(const int _nPar, const FitFunction* _f)
        : f{_f}, parameters_(_nPar) {}
    ~ParametricFunction() { delete f; }
    ParametricFunction(const ParametricFunction&) = delete;
    ParametricFunction(ParametricFunction&&) = default;

    void setSuccess(bool s) { success_ = s; }
    DoubleWithError& parameterAt(int ip) { return parameters_[ip]; }

    double y(const double x) const;
    int parameterCount() const { return parameters_.size(); }
    bool success() const { return success_; }
    const std::vector<DoubleWithError>& parameters() const { return parameters_; };

    const FitFunction* f;

private:
    std::vector<DoubleWithError> parameters_;
    bool success_ {false};
};

#endif // PARAMETRIC_FUNCTION_H

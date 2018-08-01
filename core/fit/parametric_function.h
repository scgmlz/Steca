//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.h
//! @brief     Defines classes FitParameter, ParametricFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PARAMETRIC_FUNCTION_H
#define PARAMETRIC_FUNCTION_H

#include "core/typ/range.h"

//! Parameter value, error, and allowed range for ParametricFunction

class FitParameter final {
public:
    FitParameter();

    void setValue(double value, double error);
    void reset();
    void setAllowedRange(double min, double max);

    double value() const { return value_; }
    double error() const { return error_; }
    const Range& range() const;

private:
    double value_, error_;
    Range range_; //!< allowed range of values
};

//! Abstract function with parameters

class ParametricFunction {
public:
    virtual ~ParametricFunction() {}

    //! evaluate the function y = f(x), with given (parValues) or own parameters
    virtual double y(double x, double const* parValues = nullptr) const = 0;
    //! partial derivative / parameter, with given (parValues) or own parameters
    virtual double dy(double x, int parIndex, double const* parValues = nullptr) const = 0;

    void setParameterCount(int n) { parameters_.resize(n, {}); }
    virtual void reset();
    FitParameter& parameterAt(int ip) { return parameters_[ip]; }

    int parameterCount() const { return parameters_.size(); }

protected:
    void setParValue(int ip, double val) {parameters_[ip].setValue(val, 0); }

    double parValue(int parIndex, double const* parValues) const;

    std::vector<FitParameter> parameters_;
};

#endif // PARAMETRIC_FUNCTION_H

//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.h
//! @brief     Defines class ParametricFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PARAMETRIC_FUNCTION_H
#define PARAMETRIC_FUNCTION_H

#include "core/typ/json.h"
#include "core/typ/range.h"

//! Parameter value, error, and allowed range for ParametricFunction

class FitParameter final {
public:
    FitParameter();

    double value() const { return value_; }
    double error() const { return error_; }

    double allowedMin() const;
    double allowedMax() const;
    void setAllowedRange(double min, double max);

    void setValue(double value, double error);
    void reset();

    JsonObj toJson() const;
    void fromJson(const JsonObj&);

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

public:
    void setParameterCount(int);
    int parameterCount() const;
    FitParameter& parameterAt(int);

    virtual void reset();

    virtual JsonObj toJson() const;
    virtual void fromJson(const JsonObj&);
    virtual QString name() const = 0;

protected:
    double parValue(int parIndex, double const* parValues) const;
    void setParValue(int parIndex, double val);
    QVector<FitParameter> parameters_;
};

#endif // PARAMETRIC_FUNCTION_H

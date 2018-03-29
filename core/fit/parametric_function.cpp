//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.cpp
//! @brief     Implements class ParametricFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/def/idiomatic_for.h"
#include "parametric_function.h"

//  ***********************************************************************************************
//! @class FitParameter

FitParameter::FitParameter()
    : value_(0)
    , error_(0)
    , range_(Range::infinite())
{}

double FitParameter::allowedMin() const
{
    return range_.isValid() ? range_.min : value_;
}

double FitParameter::allowedMax() const
{
    return range_.isValid() ? range_.max : value_;
}

void FitParameter::setAllowedRange(double min, double max)
{
    range_.set(min, max);
}

void FitParameter::setValue(double value, double error)
{
    value_ = value;
    error_ = error;
}

void FitParameter::reset()
{
    value_ = range_.bound(0);
    error_ = 0;
}

JsonObj FitParameter::toJson() const
{
    JsonObj ret;
    ret.insert("value", double_to_json(value_));
    ret.insert("range", range_.toJson());
    return ret;
}

void FitParameter::fromJson(const JsonObj& obj)
{
    value_ = obj.loadQreal("value");
    range_ = obj.loadRange("range");
}

//  ***********************************************************************************************
//! @class ParametricFunction

void ParametricFunction::setParameterCount(int count)
{
    parameters_.fill(FitParameter(), count);
}

int ParametricFunction::parameterCount() const
{
    return parameters_.count();
}

FitParameter& ParametricFunction::parameterAt(int i)
{
    return parameters_[i];
}

void ParametricFunction::reset()
{
    for_i (parameters_.count()) {
        auto& p = parameters_[i];
        p.reset();
    }
}

JsonObj ParametricFunction::toJson() const
{
    QJsonArray params;
    for (const FitParameter& param : parameters_)
        params.append(param.toJson());
    JsonObj ret;
    ret.insert("parameters", params);
    return ret;
}

void ParametricFunction::fromJson(const JsonObj& obj)
{
    QJsonArray params = obj.loadArr("parameters");
    int parCount = params.count();
    setParameterCount(parCount);
    for_i (parCount)
        parameters_[i].fromJson(params.at(i).toObject());
}

double ParametricFunction::parValue(int i, double const* parValues) const
{
    return parValues ? parValues[i] : parameters_.at(i).value();
}

void ParametricFunction::setParValue(int i, double val)
{
    parameters_[i].setValue(val, 0);
}

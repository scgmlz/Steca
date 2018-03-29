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
//! @class ParametricFunction::Parameter

ParametricFunction::Parameter::Parameter()
    : value_(0)
    , error_(0)
    , range_(Range::infinite())
{}

Range ParametricFunction::Parameter::valueRange() const
{
    return range_.isValid() ? range_ : Range(value_, value_);
}

void ParametricFunction::Parameter::setValueRange(double min, double max)
{
    range_.set(min, max);
}

void ParametricFunction::Parameter::setValue(double value, double error)
{
    value_ = value;
    error_ = error;
}

JsonObj ParametricFunction::Parameter::toJson() const
{
    JsonObj ret;
    ret.insert("value", double_to_json(value_));
    ret.insert("range", range_.toJson());
    return ret;
}

void ParametricFunction::Parameter::fromJson(const JsonObj& obj)
{
    value_ = obj.loadQreal("value");
    range_ = obj.loadRange("range");
}

void ParametricFunction::setParameterCount(int count)
{
    parameters_.fill(Parameter(), count);
}

int ParametricFunction::parameterCount() const
{
    return parameters_.count();
}

ParametricFunction::Parameter& ParametricFunction::parameterAt(int i)
{
    return parameters_[i];
}

void ParametricFunction::reset()
{
    for_i (parameters_.count()) {
        auto& p = parameters_[i];
        p.setValue(p.valueRange().bound(0), 0);
    }
}

JsonObj ParametricFunction::toJson() const
{
    QJsonArray params;
    for (const Parameter& param : parameters_)
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

void ParametricFunction::setValue(int i, double val)
{
    parameters_[i].setValue(val, 0);
}

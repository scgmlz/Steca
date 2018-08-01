//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.cpp
//! @brief     Implements classes FitParameter, ParametricFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "parametric_function.h"
#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class FitParameter

FitParameter::FitParameter()
    : value_(0)
    , error_(0)
    , range_(Range::infinite())
{}

void FitParameter::setAllowedRange(double min, double max)
{
    range_.set(min, max);
}

const Range& FitParameter::range() const {
    ASSERT (range_.isValid());
    return range_;
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


//  ***********************************************************************************************
//! @class ParametricFunction

void ParametricFunction::setParameterCount(int count)
{
    parameters_.resize(count, FitParameter());
}

int ParametricFunction::parameterCount() const
{
    return parameters_.size();
}

FitParameter& ParametricFunction::parameterAt(int ip)
{
    return parameters_[ip];
}

void ParametricFunction::reset()
{
    for (auto& p: parameters_)
        p.reset();
}

double ParametricFunction::parValue(int ip, double const* parValues) const
{
    return parValues ? parValues[ip] : parameters_.at(ip).value();
}

void ParametricFunction::setParValue(int ip, double val)
{
    parameters_[ip].setValue(val, 0);
}

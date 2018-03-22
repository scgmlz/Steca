// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.cpp
//! @brief     Implements class Function
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/def/idiomatic_for.h"
#include "parametric_function.h"

// ************************************************************************** //
//   class Function::Parameter
// ************************************************************************** //

Function::Parameter::Parameter() : value_(0), error_(0), range_(Range::infinite()) {}

Range Function::Parameter::valueRange() const {
    return range_.isValid() ? range_ : Range(value_, value_);
}

void Function::Parameter::setValueRange(qreal min, qreal max) {
    range_.set(min, max);
}

void Function::Parameter::setValue(qreal value, qreal error) {
    value_ = value;
    error_ = error;
}

JsonObj Function::Parameter::toJson() const {
    JsonObj ret;
    ret.insert("value", qreal_to_json(value_));
    ret.insert("range", range_.toJson());
    return ret;
}

void Function::Parameter::fromJson(const JsonObj& obj) THROWS {
    value_ = obj.loadQreal("value");
    range_ = obj.loadRange("range");
}

void Function::setParameterCount(int count) {
    parameters_.fill(Parameter(), count);
}

int Function::parameterCount() const {
    return parameters_.count();
}

Function::Parameter& Function::parameterAt(int i) {
    return parameters_[i];
}

void Function::reset() {
    for_i (parameters_.count()) {
        auto& p = parameters_[i];
        p.setValue(p.valueRange().bound(0), 0);
    }
}

JsonObj Function::toJson() const {
    QJsonArray params;
    for (const Parameter& param : parameters_)
        params.append(param.toJson());
    JsonObj ret;
    ret.insert("parameters", params);
    return ret;
}

void Function::fromJson(const JsonObj& obj) THROWS {
    QJsonArray params = obj.loadArr("parameters");
    int parCount = params.count();
    setParameterCount(parCount);
    for_i (parCount)
        parameters_[i].fromJson(params.at(i).toObject());
}

qreal Function::parValue(int i, qreal const* parValues) const {
    return parValues ? parValues[i] : parameters_.at(i).value();
}

void Function::setValue(int i, qreal val) {
    parameters_[i].setValue(val, 0);
}

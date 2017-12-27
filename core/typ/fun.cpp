// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/fun.cpp
//! @brief     Implements classes Function, SimpleFunction
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "def/idiomatic_for.h"
#include "typ/fun.h"

namespace typ {

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

JsonObj Function::Parameter::to_json() const {
    JsonObj ret;
    ret.insert("value", qreal_to_json(value_));
    ret.insert("range", range_.to_json());
    return ret;
}

void Function::Parameter::from_json(JsonObj const& obj) THROWS {
    value_ = obj.loadQreal("value");
    range_ = obj.loadRange("range");
}


// ************************************************************************** //
//   class SimpleFunction
// ************************************************************************** //

void SimpleFunction::setParameterCount(uint count) {
    parameters_.fill(Parameter(), count);
}

uint SimpleFunction::parameterCount() const {
    return parameters_.count();
}

Function::Parameter& SimpleFunction::parameterAt(uint i) {
    return parameters_[i];
}

void SimpleFunction::reset() {
    for_i (parameters_.count()) {
        auto& p = parameters_[i];
        p.setValue(p.valueRange().bound(0), 0);
    }
}

JsonObj SimpleFunction::to_json() const {
    QJsonArray params;
    for (const Parameter& param : parameters_)
        params.append(param.to_json().sup());
    JsonObj ret = super::to_json();
    ret.insert("parameters", params);
    return ret;
}

void SimpleFunction::from_json(JsonObj const& obj) THROWS {
    super::from_json(obj);
    QJsonArray params = obj.loadArr("parameters");
    uint parCount = params.count();
    setParameterCount(parCount);
    for_i (parCount)
        parameters_[i].from_json(params.at(i).toObject());
}

qreal SimpleFunction::parValue(uint i, qreal const* parValues) const {
    return parValues ? parValues[i] : parameters_.at(i).value();
}

void SimpleFunction::setValue(uint i, qreal val) {
    parameters_[i].setValue(val, 0);
}

} // namespace typ

#include "approx_function.h"

namespace core { namespace approx {

Function::Parameter::Parameter(): value(0), range() {
}

qreal Function::Parameter::getValue() const {
  return value;
}

Range Function::Parameter::getRange() const {
  return range.isValid() ? range : Range(value);
}

bool Function::Parameter::checkValue(qreal v) {
  if (range.isValid()) {
    if (!range.contains(v)) return false;
  }

  return true;
}

bool Function::Parameter::setValue(qreal value_, bool force) {
  if (force || checkValue(value_)) {
    value = value_;
    return true;
  }

  return false;
}

Function::Function() {
}

Function::~Function()
{
}

void Function::setParameterCount(uint n) {
  parameters.fill(Parameter(),n);
}

uint Function::getParameterCount() const {
  return parameters.count();
}

Functions::Functions() {
}

Functions::~Functions() {
  for (auto f: functions)
    delete f;
}

SumFunctions::SumFunctions() {
}

qreal SumFunctions::y(qreal x) const {
  qreal sum = 0;
  for (Function *f: functions.getFunctions())
    sum += f->y(x);
  return sum;
}

}}

// eof

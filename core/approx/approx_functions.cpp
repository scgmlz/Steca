#include "approx_functions.h"

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

Function::~Function() {
}

SingleFunction::SingleFunction() {
}

void SingleFunction::setParameterCount(uint count) {
  parameters.fill(Parameter(),count);
}

uint SingleFunction::getParameterCount() const {
  return parameters.count();
}

SumFunctions::SumFunctions() {
}

SumFunctions::~SumFunctions() {
  for (auto f: functions)
    delete f;
}

void SumFunctions::addFunction(Function* function) {
  ASSERT(function)
      functions.append(function);
}

uint SumFunctions::getParameterCount() const {
  uint count = 0;
  for (auto f: functions)
    count += f->getParameterCount();
  return count;
}

qreal SumFunctions::y(qreal x) const {
  qreal sum = 0;
  for (auto f: functions)
    sum += f->y(x);
  return sum;
}

Polynomial::Polynomial(uint degree) {
  setDegree(degree);
}

qreal Polynomial::y(qreal x) const {
  qreal value = 0, powX = 1;
  for (int i=0, iCount = parameters.count();;) {
    value += parameters[i].getValue() * powX;
    if (++i >= iCount) break;
    powX *= x;
  }
  return value;
}

}}

// eof

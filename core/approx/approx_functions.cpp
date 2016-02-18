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

uint SingleFunction::parameterCount() const {
  return parameters.count();
}

Function::Parameter const& SingleFunction::getParameter(uint i) const {
  return parameters.at(i);
}

SumFunctions::SumFunctions(): parCount(0) {
}

SumFunctions::~SumFunctions() {
  for (auto f: functions)
    delete f;
}

void SumFunctions::addFunction(Function* function) {
  ASSERT(function)
  functions.append(function);
  parCount += function->parameterCount();
  for_i (function->parameterCount())
    parameters.append(&function->getParameter(i));
}

uint SumFunctions::parameterCount() const {
  return parCount;
}

Function::Parameter const& SumFunctions::getParameter(uint i) const {
  return *parameters.at(i);
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

bool Polynomial::__calculateDyda(qreal x, uint positionInsideTarget, reals_t target) const {
  if ((positionInsideTarget + parameterCount()) >= target.size()) {
    NEVER_HERE // TODO unnecessary check and bool return ?
    return false;
  }

  for_i (parameterCount())
    target[positionInsideTarget + i] = pow(x,i);

  return true;
}

Curve::Point::Point(qreal x_, qreal y_, qreal tolerance_)
: x(x_), y(y_), tolerance(tolerance_) {
}

Curve::Curve() {
}

bool Curve::isEmpty() const {
  return points.isEmpty();
}

void Curve::addPoint(Point const& point) {
  domainX.extend(point.x);
  domainY.extend(point.y);
  points.append(point);
}

uint Curve::pointCount() const {
  return points.count();
}

const Curve::Point &Curve::getPoint(uint i) const {
  return points.at(i);
}

}}

// eof

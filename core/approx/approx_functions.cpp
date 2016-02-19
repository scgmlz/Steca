#include "approx_functions.h"

namespace core { namespace approx {

Function::Parameter::Parameter()
  : value(0), range(Range::infinite())
  , maxDelta(qSNaN()), maxDeltaPercent(qSNaN())
  , maxError(qSNaN()), maxErrorPercent(qSNaN()) {
}

qreal Function::Parameter::getValue() const {
  return value;
}

Range Function::Parameter::getRange() const {
  return range.isValid() ? range : Range(value);
}

bool Function::Parameter::checkValue(qreal v, qreal error) {
  if (range.isValid()) {
    if (!range.contains(v)) return false;
  }

  if (qIsFinite(maxDelta)) {
    if (qAbs(value - v) > maxDelta) return false;
  }

  if (qIsFinite(maxDeltaPercent)) {
    if (0 == value) return false;
    if (qAbs((value - v) / value) * 100 > maxDeltaPercent) return false;
  }

  if (qIsFinite(maxError)) {
    if (error > maxError) return false;
  }

  if (qIsFinite(maxErrorPercent)) {
    if (0 == value) return false;
    if (qAbs(error / value) * 100 > maxDeltaPercent) return false;
  }

  return true;
}

bool Function::Parameter::setValue(qreal value, qreal error, bool force) {
  if (force || checkValue(value, error)) {
    this->value = value;
    return true;
  }

  return false;
}

Function::Function() {
}

Function::~Function() {
}

QDebug& operator<<(QDebug& os, Function const& f) {
  auto parCount = f.parameterCount();
  os << '[' << parCount << ']';
  for_i (parCount) {
    os << i << ':' << f.getParameter(i).getValue();
  }

  return os;
}

SingleFunction::SingleFunction() {
}

void SingleFunction::setParameterCount(uint count) {
  parameters.fill(Parameter(),count);
}

uint SingleFunction::parameterCount() const {
  return parameters.count();
}

Function::Parameter& SingleFunction::getParameter(uint i) {
  return parameters[i];
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

Function::Parameter& SumFunctions::getParameter(uint i) {
  return *parameters.at(i);
}

qreal SumFunctions::y(qreal x) const {
  qreal sum = 0;
  for (auto f: functions)
    sum += f->y(x);
  return sum;
}

qreal SumFunctions::y(qreal x, const qreal *parVals) const {
  qreal sum = 0;
  for (auto f: functions) {
    sum += f->y(x,parVals);
    parVals += f->parameterCount(); // advance to next function
  }
  return sum;
}

qreal SumFunctions::dy(qreal, int) const {
  NOT_YET // TODO
}

qreal SumFunctions::dy(qreal, int, qreal const*) const {
  NOT_YET
}

Polynomial::Polynomial(uint degree) {
  setDegree(degree);
}

static qreal pow_n(qreal x, uint n) {
  qreal value = 1;
  while (n-- > 0) value *= x;
  return value;
}

qreal Polynomial::y(qreal x) const {
  qreal value = 0;
  for_i (parameters.count()) value += parameters[i].getValue() * pow_n(x,i);
  return value;
}

qreal Polynomial::y(qreal x, const qreal *parVals) const {
  qreal value = 0;
  for_i (parameters.count()) value += parVals[i] * pow_n(x,i);
  return value;
}

qreal Polynomial::dy(qreal x, int i) const {
  return pow_n(x,i);
}

qreal Polynomial::dy(qreal x, int i, qreal const*) const {
  return pow_n(x,i);
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

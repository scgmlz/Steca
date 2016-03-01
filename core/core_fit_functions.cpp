#include "core_fit_functions.h"

namespace core { namespace fit {
//------------------------------------------------------------------------------

Function::Parameter::Parameter()
  : value(0), range(Range::infinite())
  , maxDelta(qQNaN()), maxDeltaPercent(qQNaN())
  , maxError(qQNaN()), maxErrorPercent(qQNaN()) {
}

Range Function::Parameter::getRange() const {
  return range.isValid() ? range : Range(value);
}

void Function::Parameter::setRange(qreal min, qreal max) {
  range.set(min,max);
}

bool Function::Parameter::checkValue(qreal newValue, qreal newError) {
  if (range.isValid()) {
    if (!range.contains(newValue)) return false;
  }

  if (!qIsNaN(maxDelta)) {
    if (qAbs(value - newValue) > maxDelta) return false;
  }

  if (!qIsNaN(maxDeltaPercent)) {
    if (0 == value) return false;
    if (qAbs((value - newValue) / value) * 100 > maxDeltaPercent) return false;
  }

  if (!qIsNaN(maxError)) {
    if (newError > maxError) return false;
  }

  if (!qIsNaN(maxErrorPercent)) {
    if (0 == value) return false;
    if (qAbs(newError / value) * 100 > maxDeltaPercent) return false;
  }

  return true;
}

bool Function::Parameter::setValue(qreal value, qreal error, bool force) {
  if (!force && !checkValue(value, error)) return false;

  this->value = value;
  return true;
}

//------------------------------------------------------------------------------

Function::Function() {
}

Function::~Function() {
}

#ifndef QT_NO_DEBUG
QDebug& operator<<(QDebug& os, Function const& f) {
  auto parCount = f.parameterCount();
  os << str("[%1]").arg(parCount);
  for_i (parCount) {
    os << str("%1: %2").arg(i).arg(f.getParameter(i).getValue());
  }
  return os;
}
#endif

//------------------------------------------------------------------------------

SimpleFunction::SimpleFunction() {
}

void SimpleFunction::setParameterCount(uint count) {
  parameters.fill(Parameter(),count);
}

uint SimpleFunction::parameterCount() const {
  return parameters.count();
}

Function::Parameter& SimpleFunction::getParameter(uint i) {
  return parameters[i];
}

qreal SimpleFunction::parValue(uint i, const qreal *parameterValues) const {
  return parameterValues ? parameterValues[i] : parameters[i].getValue();
}

void SimpleFunction::setValue(uint i, qreal val) {
  parameters[i].setValue(val);
}

//------------------------------------------------------------------------------

SumFunctions::SumFunctions() {
}

SumFunctions::~SumFunctions() {
  // dispose of the Functions that were added
  for (Function *f: functions) delete f;
}

void SumFunctions::addFunction(Function* function) {
  ASSERT(function)

  uint parIndex = parameterCount();
  functions.append(function);

  for_i (function->parameterCount()) {
    // aggregate parameter list
    parameters.append(&function->getParameter(i));
    // lookup helpers
    function_parIndex.append(function);
    firstParIndex_parIndex.append(parIndex);
  }
}

uint SumFunctions::parameterCount() const {
  return parameters.count();
}

Function::Parameter& SumFunctions::getParameter(uint i) {
  return *parameters.at(i);
}

qreal SumFunctions::y(qreal x, const qreal *parValues) const {
  qreal sum = 0;
  for (Function *f: functions) {
    sum += f->y(x,parValues);
    if (parValues)
      parValues += f->parameterCount(); // advance to next function
  }
  return sum;
}

qreal SumFunctions::dy(qreal x, uint parIndex, qreal const* parValues) const {
  Function *f = function_parIndex.at(parIndex); // aggregate parIndex refers to function f

  // offset parameter index
  uint firstIndex = firstParIndex_parIndex[parIndex];
  if (parValues) parValues += firstIndex;

  ASSERT(firstIndex <= parIndex)
  parIndex -= firstIndex;

  ASSERT(parIndex < f->parameterCount())

  return f->dy(x, parIndex, parValues);
}

//------------------------------------------------------------------------------

Polynomial::Polynomial(uint degree) {
  setDegree(degree);
}

uint Polynomial::getDegree() const {
  uint parCount = super::parameterCount();
  ASSERT(parCount > 0)
  return parCount - 1;
}

void Polynomial::setDegree(uint degree) {
  super::setParameterCount(degree+1);
}

// the power with *uint* exponent
static qreal pow_n(qreal x, uint n) {
  qreal value = 1;
  while (n-- > 0) value *= x;
  return value;
}

qreal Polynomial::y(qreal x, const qreal *parameterValues) const {
  qreal value = 0;
  for_i (parameters.count())
    value += parValue(i,parameterValues) * pow_n(x,i);
  return value;
}

qreal Polynomial::dy(qreal x, uint i, qreal const*) const {
  return pow_n(x,i);
}

//------------------------------------------------------------------------------

PeakFunction::PeakFunction() {
}

//------------------------------------------------------------------------------

Gaussian::Gaussian(qreal ampl, qreal mu, qreal sigma) {
  setParameterCount(3);

  auto &parAmpl  = parameters[parAMPL];
  auto &parMu    = parameters[parMU];
  auto &parSigma = parameters[parSIGMA];

  parAmpl.setValue(ampl);
  parAmpl.setRange(0,qInf());

  parMu.setValue(mu);

  parSigma.setValue(sigma);
  parSigma.setRange(0,qInf());
}

qreal Gaussian::y(qreal x, const qreal *parameterValues) const {
  qreal ampl  = parValue(parAMPL,  parameterValues);
  qreal mu    = parValue(parMU,    parameterValues);
  qreal sigma = parValue(parSIGMA, parameterValues);

  qreal arg = (x - mu) / sigma;
  return ampl * exp(-0.5 * arg * arg);
}

qreal Gaussian::dy(qreal x, uint parameterIndex, const qreal *parameterValues) const {
  qreal ampl  = parValue(parAMPL,  parameterValues);
  qreal mu    = parValue(parMU,    parameterValues);
  qreal sigma = parValue(parSIGMA, parameterValues);

  qreal arg = (x - mu) / sigma;
  qreal ex  = exp(-0.5 * arg * arg);

  switch (parameterIndex) {
  case parAMPL: // ampl
    return ex;
  case parMU: // mu
    return ampl * ex * (x-mu)/(sigma*sigma);
  case parSIGMA: // sigma
    return ampl * ex * ((x-mu)*(x-mu))/(sigma*sigma*sigma);
  default:
    NEVER_HERE return 0;
  }
}

void Gaussian::setPeak(qreal tth, qreal intens) {
  setValue(parAMPL, intens);
  setValue(parMU,   tth);
}

void Gaussian::setFWHM(qreal val) {
  // sigma = FWHM * 1/4 * (SQRT(2)/SQRT(ln(2))) = FWHM * 0.424661
  setValue(parSIGMA, val * 0.424661);
}

//------------------------------------------------------------------------------
}}
// eof

#include "core_fit_functions.h"
#include <QJsonObject>
#include <cmath>

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

void Function::Parameter::loadFrom(QJsonObject const& obj) {
  value = obj["value"].toDouble(); // TODO etc.

}

void Function::Parameter::saveTo(QJsonObject& obj) const {
  obj["value"] = value; // TODO etc..
}

//------------------------------------------------------------------------------

Function::Function() {
}

Function::~Function() {
}

void Function::loadFrom(QJsonObject const& obj) /*TODO: THROWS*/ {
  // RUNTIME_CHECK par.count
  for_i (parameterCount()) {
    getParameter(i).loadFrom(obj[str("p")+(i+1)].toObject());
  }
}

void Function::saveTo(QJsonObject& obj) const {
  auto parCount = parameterCount();
  obj["??"] = (int)parCount;
  for_i (parCount) {
    QJsonObject pObj;
    getParameter(i).saveTo(pObj);
    obj[str("p")+(i+1)] = pObj;
  }
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

void SimpleFunction::reset() {
  for_i (parameters.count()) {
    auto &p = parameters[i];
    p.setValue(p.getRange().bound(0));
  }
}

void SimpleFunction::loadFrom(QJsonObject const& obj) {
  int parCount = obj["??"].toInt();
//  RUNTIME_CHECK(parCount>0)
  setParameterCount(parCount);
  super::loadFrom(obj);
}

void SimpleFunction::saveTo(QJsonObject& obj) const {
  super::saveTo(obj);
  // nothing else to do here
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

void SumFunctions::loadFrom(QJsonObject const&) {
  // load number of functions; for (n) addFunction/loadFrom
}

void SumFunctions::saveTo(QJsonObject&) const {
  //
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

void Polynomial::loadFrom(QJsonObject const&) {
  // RUNTIME_CHECK obj["type"] == "polynomial"
}

void Polynomial::saveTo(QJsonObject&) const {
  // obj["type"] = "polynomial" ; super::saveTo
}

//------------------------------------------------------------------------------

PeakFunction::PeakFunction() {
}

//------------------------------------------------------------------------------

Gaussian::Gaussian(qreal ampl, qreal xShift, qreal sigma) {
  setParameterCount(3);

  auto &parAmpl   = parameters[parAMPL];
  auto &parXShift = parameters[parXSHIFT];
  auto &parSigma  = parameters[parSIGMA];

  parAmpl.setRange(0,qInf());
  parAmpl.setValue(ampl);

  parXShift.setValue(xShift);

  parSigma.setRange(0,qInf());
  parSigma.setValue(sigma);
}

qreal Gaussian::y(qreal x, const qreal *parameterValues) const {
  qreal ampl   = parValue(parAMPL,   parameterValues);
  qreal xShift = parValue(parXSHIFT, parameterValues);
  qreal sigma  = parValue(parSIGMA,  parameterValues);

  qreal arg = (x - xShift) / sigma;
  qreal exa = exp(-0.5 * arg * arg);

  return ampl * exa;
}

qreal Gaussian::dy(qreal x, uint parameterIndex, const qreal *parameterValues) const {
  qreal ampl   = parValue(parAMPL,   parameterValues);
  qreal xShift = parValue(parXSHIFT, parameterValues);
  qreal sigma  = parValue(parSIGMA,  parameterValues);

  qreal arg = (x - xShift) / sigma;
  qreal exa = exp(-0.5 * arg * arg);

  switch (parameterIndex) {
  case parAMPL:
    return exa;
  case parXSHIFT:
    return ampl * exa * (x-xShift)/(sigma*sigma);
  case parSIGMA:
    return ampl * exa * ((x-xShift)*(x-xShift))/(sigma*sigma*sigma);
  default:
    NEVER_HERE return 0;
  }
}

void Gaussian::setPeak(qreal x, qreal y) {
  setValue(parXSHIFT, x);
  setValue(parAMPL,   y);
}

void Gaussian::setFWHM(qreal val) {
  // sigma = FWHM * 1/4 * (SQRT(2)/SQRT(ln(2))) = FWHM * 0.424661
  setValue(parSIGMA, val * 0.424661);
}

//------------------------------------------------------------------------------

CauchyLorentz::CauchyLorentz(qreal ampl, qreal xShift, qreal gamma) {
  setParameterCount(3);

  auto &parAmpl   = parameters[parAMPL];
  auto &parXShift = parameters[parXSHIFT];
  auto &parGamma  = parameters[parGAMMA];

  parAmpl.setRange(0,qInf());
  parAmpl.setValue(ampl);

  parXShift.setValue(xShift);

  parGamma.setRange(0,qInf());
  parGamma.setValue(gamma);
}

qreal CauchyLorentz::y(qreal x, const qreal *parameterValues) const {
  qreal ampl   = parValue(parAMPL,   parameterValues);
  qreal xShift = parValue(parXSHIFT, parameterValues);
  qreal gamma  = parValue(parGAMMA,  parameterValues);

  qreal arg = (x - xShift) / gamma;
  return ampl / (1 + arg * arg);
}

qreal CauchyLorentz::dy(qreal x, uint parameterIndex, const qreal *parameterValues) const {
  qreal ampl   = parValue(parAMPL,   parameterValues);
  qreal xShift = parValue(parXSHIFT, parameterValues);
  qreal gamma  = parValue(parGAMMA,  parameterValues);

  qreal arg1 = (x - xShift) / gamma;
  qreal arg2 = arg1 * arg1;
  qreal arg3 = (1 + arg2) * (1 + arg2);

  switch (parameterIndex) {
  case parAMPL:
    return 1 / (1 + arg2);
  case parXSHIFT:
    return  2 * ampl * (x-xShift) / (arg3 * gamma * gamma);
  case parGAMMA:
    return  2 * ampl * (x-xShift) * (x-xShift) / (arg3 * gamma * gamma * gamma);
  default:
    NEVER_HERE return 0;
  }
}

void CauchyLorentz::setPeak(qreal x, qreal y) {
  setValue(parXSHIFT, x);
  setValue(parAMPL,   y);
}

void CauchyLorentz::setFWHM(qreal val) {
  // gamma = HWHM = FWHM / 2
  setValue(parGAMMA, val / 2);
}

//------------------------------------------------------------------------------

PseudoVoigt1::PseudoVoigt1(qreal ampl, qreal xShift, qreal sigmaGamma, qreal eta) {
  setParameterCount(4);

  auto &parAmpl       = parameters[parAMPL];
  auto &parXShift     = parameters[parXSHIFT];
  auto &parSigmaGamma = parameters[parSIGMAGAMMA];
  auto &parEta        = parameters[parETA];

  parAmpl.setRange(0,qInf());
  parAmpl.setValue(ampl);

  parXShift.setValue(xShift);

  parSigmaGamma.setRange(0,qInf());
  parSigmaGamma.setValue(sigmaGamma);

  parEta.setRange(0,1);
  parEta.setValue(eta);
}

qreal PseudoVoigt1::y(qreal x, const qreal* parameterValues) const {
  qreal ampl       = parValue(parAMPL,       parameterValues);
  qreal xShift     = parValue(parXSHIFT,     parameterValues);
  qreal sigmaGamma = parValue(parSIGMAGAMMA, parameterValues);
  qreal eta        = parValue(parETA,        parameterValues);

  qreal arg      = (x - xShift) / sigmaGamma;
  qreal arg2     = arg * arg;
  qreal gaussian = ampl * exp(-arg2 * log(2.0));
  qreal lorentz  = ampl / (1 + arg2);

  return (1-eta) * gaussian + eta * lorentz;
}

qreal PseudoVoigt1::dy(qreal x, uint parameterIndex, const qreal* parameterValues) const {
  qreal ampl       = parValue(parAMPL,       parameterValues);
  qreal xShift     = parValue(parXSHIFT,     parameterValues);
  qreal sigmaGamma = parValue(parSIGMAGAMMA, parameterValues);
  qreal eta        = parValue(parETA,        parameterValues);

  qreal arg1 = (x - xShift) / sigmaGamma;
  qreal arg2 = arg1 * arg1;
  qreal arg3 = exp(-arg2 * log(2.0));
  qreal arg4 = 1 + arg2;

  switch (parameterIndex) {
  case parAMPL:
    return eta / arg4 + (1 - eta) * arg3;
  case parXSHIFT:
    return eta * 2 * ampl * (x-xShift) / (arg4*arg4 * sigmaGamma*sigmaGamma)
        + (1-eta) * 2 * ampl * (x-xShift) * log(2.0) * arg3 / (sigmaGamma*sigmaGamma);
  case parSIGMAGAMMA:
    return eta * 2 * ampl * (x-xShift) * (x-xShift) / (arg4*arg4 * sigmaGamma*sigmaGamma*sigmaGamma)
        + (1-eta) * 2 * ampl * (x-xShift) * (x-xShift) * log(2.0) * arg3 / (sigmaGamma*sigmaGamma*sigmaGamma);
  case parETA:
    return ampl/arg4 - ampl*arg3;
  default:
    NEVER_HERE return 0;
  }
}

void PseudoVoigt1::setPeak(qreal x, qreal y) {
  setValue(parXSHIFT, x);
  setValue(parAMPL,   y);
}

void PseudoVoigt1::setFWHM(qreal val) {
  setValue(parSIGMAGAMMA, val / 2);
}

//------------------------------------------------------------------------------

PseudoVoigt2::PseudoVoigt2(qreal ampl, qreal mu, qreal hwhmG, qreal hwhmL, qreal eta) {
  setParameterCount(5);

  auto &parAmpl  = parameters[parAMPL];
  auto &parMu    = parameters[parXSHIFT];
  auto &parHwhmG = parameters[parSIGMA];
  auto &parHwhmL = parameters[parGAMMA];
  auto &parEta   = parameters[parETA];

  parAmpl.setRange(0,qInf());
  parAmpl.setValue(ampl);

  parMu.setValue(mu);

  parHwhmG.setRange(0,qInf());
  parHwhmG.setValue(hwhmG);

  parHwhmL.setRange(0,qInf());
  parHwhmL.setValue(hwhmL);

  parEta.setRange(0,1);
  parEta.setValue(eta);
}

qreal PseudoVoigt2::y(qreal x, const qreal* parameterValues) const {
  qreal ampl   = parValue(parAMPL,   parameterValues);
  qreal xShift = parValue(parXSHIFT, parameterValues);
  qreal sigma  = parValue(parSIGMA,  parameterValues);
  qreal gamma  = parValue(parGAMMA,  parameterValues);
  qreal eta    = parValue(parETA,    parameterValues);

  qreal argG     = (x - xShift) / sigma;
  qreal argG2    = argG * argG;
  qreal gaussian = ampl * exp(-argG2 * log(2.0));

  qreal argL     = (x - xShift) / gamma;
  qreal argL2    = argL * argL;
  qreal lorentz  = ampl / (1 + argL2);

  return (1-eta) * gaussian + eta * lorentz;
}

qreal PseudoVoigt2::dy(qreal x, uint parameterIndex, const qreal* parameterValues) const {
  qreal ampl   = parValue(parAMPL,   parameterValues);
  qreal xShift = parValue(parXSHIFT, parameterValues);
  qreal sigma  = parValue(parSIGMA,  parameterValues);
  qreal gamma  = parValue(parGAMMA,  parameterValues);
  qreal eta    = parValue(parETA,    parameterValues);

  qreal argG1 = (x - xShift) / sigma;
  qreal argG2 = argG1 * argG1;
  qreal argG3 = exp(-argG2 * log(2.0));

  qreal argL1 = (x - xShift) / gamma;
  qreal argL2 = argL1 * argL1;
  qreal argL3 = 1 + argL2;

  switch (parameterIndex) {
  case parAMPL:
    return eta / argL3 + (1 - eta) * argG3;
  case parXSHIFT:
    return eta * 2 * ampl * (x-xShift) / (argL3*argL3 * gamma*gamma)
        + (1-eta) * 2 * ampl * (x-xShift) * log(2.0) * argG3 / (sigma*sigma);
  case parSIGMA:
    return (1-eta) * 2 * ampl * (x-xShift) * (x-xShift) * log(2.0) * argG3 / (sigma*sigma*sigma);
  case parGAMMA:
    return eta * 2 * ampl * (x-xShift) * (x-xShift) / (argL3*argL3 * gamma*gamma*gamma);
  case parETA:
    return ampl/argL3 - ampl*argG3;
  default:
    NEVER_HERE return 0;
  }
}

void PseudoVoigt2::setPeak(qreal x, qreal y) {
  setValue(parXSHIFT, x);
  setValue(parAMPL,   y);
}

void PseudoVoigt2::setFWHM(qreal val) {
  setValue(parSIGMA, val * 0.424661);
  setValue(parGAMMA, val / 2);
}

//------------------------------------------------------------------------------
}}
// eof

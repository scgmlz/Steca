#include "core_fit_functions.h"
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

void SimpleFunction::reset() {
  for_i (parameters.count()) {
    auto &p = parameters[i];
    p.setValue(p.getRange().bound(0));
  }
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

  parAmpl.setRange(0,qInf());
  parAmpl.setValue(ampl);

  parMu.setValue(mu);

  parSigma.setRange(0,qInf());
  parSigma.setValue(sigma);
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

CauchyLorentz::CauchyLorentz(qreal ampl, qreal tth0, qreal gamma) {
  setParameterCount(3);

  auto &parAmpl  = parameters[parAMPL];
  auto &parTth0  = parameters[parTTH0];
  auto &parGamma = parameters[parGAMMA];

  parAmpl.setRange(0,qInf());
  parAmpl.setValue(ampl);

  parTth0.setValue(tth0);

  parGamma.setRange(0,qInf());
  parGamma.setValue(gamma);
}

qreal CauchyLorentz::y(qreal x, const qreal *parameterValues) const {
  qreal ampl  = parValue(parAMPL,  parameterValues);
  qreal tth0  = parValue(parTTH0,  parameterValues);
  qreal gamma = parValue(parGAMMA, parameterValues);

  qreal arg = (x - tth0) / gamma;
  return ampl / (1 + arg * arg);
}

qreal CauchyLorentz::dy(qreal x, uint parameterIndex, const qreal *parameterValues) const {
  qreal ampl  = parValue(parAMPL,  parameterValues);
  qreal tth0  = parValue(parTTH0,  parameterValues);
  qreal gamma = parValue(parGAMMA, parameterValues);

  qreal arg1 = (x - tth0) / gamma;
  qreal arg2 = arg1 * arg1;
  qreal arg3 = (1 + arg2) * (1 + arg2);

  switch (parameterIndex) {
  case parAMPL:
    return 1 / (1 + arg2);
  case parTTH0:
    return  2 * ampl * (x-tth0) / (arg3 * gamma * gamma);
  case parGAMMA:
    return  2 * ampl * (x-tth0) * (x-tth0) / (arg3 * gamma * gamma * gamma);
  default:
    NEVER_HERE return 0;
  }
}

void CauchyLorentz::setPeak(qreal tth0, qreal intens) {
  setValue(parAMPL, intens);
  setValue(parTTH0, tth0);
}

void CauchyLorentz::setFWHM(qreal val) {
  // gamma = HWHM = FWHM / 2
  setValue(parGAMMA, val / 2);
}

//------------------------------------------------------------------------------

PseudoVoigt1::PseudoVoigt1(qreal ampl, qreal mu, qreal hwhm, qreal eta) {
  setParameterCount(4);

  auto &parAmpl = parameters[parAMPL];
  auto &parMu   = parameters[parMU];
  auto &parHwhm = parameters[parHWHM];
  auto &parEta  = parameters[parETA];

  parAmpl.setRange(0,qInf());
  parAmpl.setValue(ampl);

  parMu.setValue(mu);

  parHwhm.setRange(0,qInf());
  parHwhm.setValue(hwhm);

  parEta.setRange(0,1);
  parEta.setValue(eta);
}

qreal PseudoVoigt1::y(qreal x, const qreal* parameterValues) const {
  qreal ampl = parValue(parAMPL, parameterValues);
  qreal mu   = parValue(parMU,   parameterValues);
  qreal hwhm = parValue(parHWHM, parameterValues);
  qreal eta  = parValue(parETA,  parameterValues);

  qreal arg      = (x - mu) / hwhm;
  qreal arg2     = arg * arg;
  qreal gaussian = ampl * exp(-arg2 * log(2.0));
  qreal lorentz  = ampl / (1 + arg2);

  return eta * lorentz + (1-eta) * gaussian;
}

qreal PseudoVoigt1::dy(qreal x, uint parameterIndex, const qreal* parameterValues) const {
  qreal ampl = parValue(parAMPL, parameterValues);
  qreal mu   = parValue(parMU,   parameterValues);
  qreal hwhm = parValue(parHWHM, parameterValues);
  qreal eta  = parValue(parETA,  parameterValues);

  qreal arg1 = (x - mu) / hwhm;
  qreal arg2 = arg1 * arg1;
  qreal arg3 = exp(-arg2 * log(2.0));
  qreal arg4 = 1 + arg2;

  switch (parameterIndex) {
  case parAMPL:
    return eta / arg4 + (1 - eta) * arg3;
  case parMU:
    return eta * 2 * ampl * (x-mu) / (arg4*arg4 * hwhm*hwhm)
        + (1-eta) * 2 * ampl * (x-mu) * log(2.0) * arg3 / (hwhm*hwhm);
  case parHWHM:
    return eta * 2 * ampl * (x-mu) * (x-mu) / (arg4*arg4 * hwhm*hwhm*hwhm)
        + (1-eta) * 2 * ampl * (x-mu) * (x-mu) * log(2.0) * arg3 / (hwhm*hwhm*hwhm);
  case parETA:
    return ampl/arg4 - ampl*arg3;
  default:
    NEVER_HERE return 0;
  }
}

void PseudoVoigt1::setPeak(qreal tth, qreal intens) {
  setValue(parAMPL, intens);
  setValue(parMU,   tth);
}

void PseudoVoigt1::setFWHM(qreal val) {
  setValue(parHWHM, val / 2);
}

//------------------------------------------------------------------------------

PseudoVoigt2::PseudoVoigt2(qreal ampl, qreal mu, qreal hwhmG, qreal hwhmL, qreal eta) {
  setParameterCount(5);

  auto &parAmpl  = parameters[parAMPL];
  auto &parMu    = parameters[parMU];
  auto &parHwhmG = parameters[parHWHMG];
  auto &parHwhmL = parameters[parHWHML];
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
  qreal ampl  = parValue(parAMPL,  parameterValues);
  qreal mu    = parValue(parMU,    parameterValues);
  qreal hwhmG = parValue(parHWHMG, parameterValues);
  qreal hwhmL = parValue(parHWHML, parameterValues);
  qreal eta   = parValue(parETA,   parameterValues);

  qreal gaussianARG   = (x - mu) / hwhmG;
  qreal gaussianARG2  = gaussianARG * gaussianARG;
  qreal gaussian      = ampl * exp(-gaussianARG2 * log(2.0));

  qreal lorentzARG    = (x - mu) / hwhmL;
  qreal lorentzARG2   = lorentzARG * lorentzARG;
  qreal lorentz       = ampl / (1 + lorentzARG2);

  return (1-eta) * gaussian + eta * lorentz;
}

qreal PseudoVoigt2::dy(qreal x, uint parameterIndex, const qreal* parameterValues) const {
  qreal ampl  = parValue(parAMPL,  parameterValues);
  qreal mu    = parValue(parMU,    parameterValues);
  qreal hwhmG = parValue(parHWHMG, parameterValues);
  qreal hwhmL = parValue(parHWHML, parameterValues);
  qreal eta   = parValue(parETA,   parameterValues);

  qreal gaussianARG  = (x - mu) / hwhmG;
  qreal gaussianARG2 = gaussianARG * gaussianARG;
  qreal gaussianARG3 = exp(-gaussianARG2 * log(2.0));

  qreal lorentzARG   = (x - mu) / hwhmL;
  qreal lorentzARG2  = lorentzARG * lorentzARG;
  qreal lorentzARG3  = 1 + lorentzARG2;

  switch (parameterIndex) {
  case parAMPL:
    return eta / lorentzARG3 + (1 - eta) * gaussianARG3;
  case parMU:
    return eta * 2 * ampl * (x-mu) / (lorentzARG3*lorentzARG3 * hwhmL*hwhmL)
        + (1-eta) * 2 * ampl * (x-mu) * log(2.0) * gaussianARG3 / (hwhmG*hwhmG);
  case parHWHMG:
    return (1-eta) * 2 * ampl * (x-mu) * (x-mu) * log(2.0) * gaussianARG3 / (hwhmG*hwhmG*hwhmG);
  case parHWHML:
    return eta * 2 * ampl * (x-mu) * (x-mu) / (lorentzARG3*lorentzARG3 * hwhmL*hwhmL*hwhmL);
  case parETA:
    return ampl/lorentzARG3 - ampl*gaussianARG3;
  default:
    NEVER_HERE return 0;
  }
}

void PseudoVoigt2::setPeak(qreal tth, qreal intens) {
  setValue(parAMPL, intens);
  setValue(parMU,   tth);
}

void PseudoVoigt2::setFWHM(qreal val) {
  setValue(parHWHMG, val / 2);
  setValue(parHWHML, val / 2);
}

//------------------------------------------------------------------------------
}}
// eof

// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_fit_functions.h
//! @brief     Functions for data fitting
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_fit_fitting.h"
#include "core_fit_functions.h"
#include "types/core_json.h"
#include <QScopedPointer>
#include <qmath.h>

namespace core { namespace fit {
//------------------------------------------------------------------------------

Function::Parameter::Parameter()
  : val(0), range(Range::infinite())
  , maxDelta(qQNaN()), maxDeltaPercent(qQNaN())
  , maxError(qQNaN()), maxErrorPercent(qQNaN()) {
}

Range Function::Parameter::valueRange() const {
  return range.isValid() ? range : Range(val);
}

void Function::Parameter::setValueRange(qreal min, qreal max) {
  range.set(min,max);
}

bool Function::Parameter::checkConstraints(qreal val_, qreal error) {
  if (range.isValid() && !range.contains(val_))
    return false;

  if (!qIsNaN(maxDelta) && qAbs(val_ - val) > maxDelta)
    return false;

  if (!qIsNaN(maxDeltaPercent) &&
      (0 == val || qAbs((val_ - val) / val) * 100 > maxDeltaPercent))
    return false;

  if (!qIsNaN(maxError) && error > maxError)
    return false;

  if (!qIsNaN(maxErrorPercent) &&
      (0 == val || qAbs(error / val) * 100 > maxErrorPercent))
    return false;

  return true;
}

bool Function::Parameter::setValue(qreal val_, qreal error, bool force) {
  if (!force && !checkConstraints(val_, error))
    return false;

  val = val_;
  return true;
}

static str const
  KEY_VALUE("value"), KEY_RANGE("range"),
  KEY_MAX_DELTA("maxDelta"), KEY_MAX_DELTA_PERCENT("maxDeltaPercent"),
  KEY_MAX_ERROR("maxError"), KEY_MAX_ERROR_PERCENT("maxErrorPercent");

JsonObj Function::Parameter::saveJson() const {
  return JsonObj()
    .saveReal(KEY_VALUE,  val)
    .saveRange(KEY_RANGE, range)

    .saveReal(KEY_MAX_DELTA,         maxDelta)
    .saveReal(KEY_MAX_DELTA_PERCENT, maxDeltaPercent)
    .saveReal(KEY_MAX_ERROR,         maxError)
    .saveReal(KEY_MAX_ERROR_PERCENT, maxErrorPercent);
}

void Function::Parameter::loadJson(rcJsonObj obj) THROWS {
  val   = obj.loadReal(KEY_VALUE);
  range = obj.loadRange(KEY_RANGE);

  maxDelta        = obj.loadReal(KEY_MAX_DELTA);
  maxDeltaPercent = obj.loadReal(KEY_MAX_DELTA_PERCENT);
  maxError        = obj.loadReal(KEY_MAX_ERROR);
  maxErrorPercent = obj.loadReal(KEY_MAX_ERROR_PERCENT);
}

//------------------------------------------------------------------------------

static str const
  KEY_SUM_FUNCTIONS("Sum of functions"),
  KEY_POLYNOMIAL("polynomial"),
  KEY_GAUSSIAN("Gaussian"), KEY_LORENTZIAN("Lorentzian"),
  KEY_PSEUDOVOIGT1("PseudoVoigt1"), KEY_PSEUDOVOIGT2("PseudoVoigt2");

Function* Function::factory(rcstr type) {
  if (KEY_SUM_FUNCTIONS == type)
    return new SumFunctions();
  if (KEY_POLYNOMIAL == type)
    return new Polynomial();
  if (KEY_GAUSSIAN == type)
    return new Gaussian();
  if (KEY_LORENTZIAN == type)
    return new CauchyLorentz();
  if (KEY_PSEUDOVOIGT1 == type)
    return new PseudoVoigt1();
  if (KEY_PSEUDOVOIGT2 == type)
    return new PseudoVoigt2();
  NEVER_HERE
    return nullptr;
}

Function::Function() {
}

Function::~Function() {
}

JsonObj Function::saveJson() const {
  // nothing to do
  return JsonObj();
}

void Function::loadJson(rcJsonObj) THROWS {
  // nothing to do
}

#ifndef QT_NO_DEBUG
QDebug& operator<<(QDebug& os, Function const& f) {
  auto parCount = f.parameterCount();
  os << str("[%1]").arg(parCount);
  for_i (parCount) {
    os << str("%1: %2").arg(i).arg(f.parameterAt(i).value());
  }
  return os;
}
#endif

//------------------------------------------------------------------------------

SimpleFunction::SimpleFunction() {
}

void SimpleFunction::setParameterCount(uint count) {
  ASSERT(count > 0)
  parameters.fill(Parameter(),count);
}

uint SimpleFunction::parameterCount() const {
  return parameters.count();
}

Function::Parameter& SimpleFunction::parameterAt(uint i) {
  return parameters[i];
}

void SimpleFunction::reset() {
  for_i (parameters.count()) {
    auto &p = parameters[i];
    p.setValue(p.valueRange().bound(0));
  }
}


static str const
  KEY_PAR_COUNT("paramater count"), KEY_PARAMETER("p%1");

JsonObj SimpleFunction::saveJson() const {
  JsonObj obj;

  auto parCount = parameters.count();
  obj.saveUint(KEY_PAR_COUNT,parCount);

  for_i (parCount)
    obj.saveObj(KEY_PARAMETER.arg(i+1), parameters[i].saveJson());

  return super::saveJson() + obj;
}

void SimpleFunction::loadJson(rcJsonObj obj) THROWS {
  super::loadJson(obj);

  uint parCount = obj.loadUint(KEY_PAR_COUNT);
  setParameterCount(parCount);

  for_i (parCount)
    parameters[i].loadJson(obj.loadObj(KEY_PARAMETER.arg(i+1)));
}

qreal SimpleFunction::parValue(uint i, qreal const* parameterValues) const {
  return parameterValues ? parameterValues[i] : parameters[i].value();
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
    parameters.append(&function->parameterAt(i));
    // lookup helpers
    function_parIndex.append(function);
    firstParIndex_parIndex.append(parIndex);
  }
}

uint SumFunctions::parameterCount() const {
  return parameters.count();
}

Function::Parameter& SumFunctions::parameterAt(uint i) {
  return *parameters.at(i);
}

qreal SumFunctions::y(qreal x, qreal const* parValues) const {
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

static str const
  KEY_FUNCTIONS_COUNT("function count"), KEY_FUNCTION("f%1"), KEY_TYPE("type");

JsonObj SumFunctions::saveJson() const {
  JsonObj obj;
  obj.saveString(KEY_TYPE, KEY_SUM_FUNCTIONS);

  uint funCount = functions.count();
  obj.saveUint(KEY_FUNCTIONS_COUNT,funCount);

  for_i (funCount)
    obj.saveObj(KEY_FUNCTION.arg(i+1), functions[i]->saveJson());

  return super::saveJson() + obj;
}

void SumFunctions::loadJson(rcJsonObj obj) THROWS {
  RUNTIME_CHECK(functions.isEmpty(),"non-empty sum of functions; cannot load twice");

  super::loadJson(obj);

  uint funCount = obj.loadUint(KEY_FUNCTIONS_COUNT);

  for_i (funCount) {
    auto funObj = obj.loadObj(KEY_FUNCTION.arg(i+1));
    QScopedPointer<Function> f(Function::factory(funObj.loadString(KEY_TYPE)));
    f->loadJson(funObj);
    addFunction(f.take());
  }
}

//------------------------------------------------------------------------------

Polynomial::Polynomial(uint degree) {
  setDegree(degree);
}

uint Polynomial::degree() const {
  uint parCount = super::parameterCount();
  ASSERT(parCount > 0)
  return parCount - 1;
}

void Polynomial::setDegree(uint degree) {
  super::setParameterCount(degree+1);
}

// the power with *uint* exponent
static qreal pow_n(qreal x, uint n) {
  qreal val = 1;
  while (n-- > 0) val *= x;
  return val;
}

qreal Polynomial::y(qreal x, const qreal *parameterValues) const {
  qreal val = 0;
  for_i (parameters.count())
    val += parValue(i,parameterValues) * pow_n(x,i);
  return val;
}

qreal Polynomial::dy(qreal x, uint i, qreal const*) const {
  return pow_n(x,i);
}

qreal Polynomial::calAverageValue(Range tth) {
  qreal average = 0;
  qreal lower = 0;
  qreal upper = 0;
  int pow   = 1;
  for_i(parameters.count()) {
    auto val = parameterAt(i).value();
    qreal powFac = (1/(qreal)pow);
    lower += powFac*val * pow_n(tth.min,pow);
    upper += powFac*val * pow_n(tth.max,pow);
    pow++;
  }
  average = (1/tth.width())*(upper-lower);

  return average;
}

JsonObj Polynomial::saveJson() const {
  JsonObj obj;
  obj.saveString(KEY_TYPE, KEY_POLYNOMIAL);
  return super::saveJson() + obj;
}

void Polynomial::loadJson(rcJsonObj obj) THROWS {
  super::loadJson(obj);
}

//------------------------------------------------------------------------------

PeakFunction* PeakFunction::factory(eType type) {
  switch (type) {
  case PEAK_GAUSSIAN:
    return new Gaussian();
  case PEAK_LORENTZIAN:
    return new CauchyLorentz();
  case PEAK_PSEUDOVOIGT1:
    return new PseudoVoigt1();
  case PEAK_PSEUDOVOIGT2:
    return new PseudoVoigt2();
  default:
    NEVER_HERE
    return nullptr;
  }
}

PeakFunction::PeakFunction(): guessPeak(), guessFwhm(qQNaN()) {
}

void PeakFunction::setGuessPeak(XY const& peak) {
  guessPeak = peak;
}

void PeakFunction::setGuessFWHM(qreal fwhm) {
  guessFwhm = fwhm;
}

void PeakFunction::reset() {
  super::reset();
  setGuessPeak(getGuessPeak());
  setGuessFWHM(getGuessFWHM());
}

static str const KEY_GUESS_FWHM("guess_fwhm");

JsonObj PeakFunction::saveJson() const {
  return super::saveJson()
      + guessPeak.saveJson().saveReal(KEY_GUESS_FWHM,guessFwhm);
}

void PeakFunction::loadJson(rcJsonObj obj) THROWS {
  super::loadJson(obj);
  guessPeak.loadJson(obj);
  guessFwhm = obj.loadReal(KEY_GUESS_FWHM);
}

//------------------------------------------------------------------------------

Gaussian::Gaussian(qreal ampl, qreal xShift, qreal sigma) {
  setParameterCount(3);

  auto &parAmpl   = parameters[parAMPL];
  auto &parXShift = parameters[parXSHIFT];
  auto &parSigma  = parameters[parSIGMA];

  parAmpl.setValueRange(0,qInf());
  parAmpl.setValue(ampl);

  parXShift.setValue(xShift);

  parSigma.setValueRange(0,qInf());
  parSigma.setValue(sigma);
}

qreal Gaussian::y(qreal x, qreal const* parameterValues) const {
  qreal ampl   = parValue(parAMPL,   parameterValues);
  qreal xShift = parValue(parXSHIFT, parameterValues);
  qreal sigma  = parValue(parSIGMA,  parameterValues);

  qreal arg = (x - xShift) / sigma;
  qreal exa = exp(-0.5 * arg * arg);

  return ampl * exa;
}

qreal Gaussian::dy(qreal x, uint parameterIndex, qreal const* parameterValues) const {
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

void Gaussian::setGuessPeak(XY const& xy) {
  super::setGuessPeak(xy);
  setValue(parXSHIFT, xy.x);
  setValue(parAMPL,   xy.y);
}

void Gaussian::setGuessFWHM(qreal val) {
  super::setGuessFWHM(val);
  // sigma = FWHM * 1/4 * (SQRT(2)/SQRT(ln(2))) = FWHM * 0.424661
  setValue(parSIGMA, val * 0.424661);
}

XY Gaussian::getFitPeak() const {
  return XY(parameters[parXSHIFT].value(), parameters[parAMPL].value());
}

qreal Gaussian::getFitFWHM() const {
  return parameters[parSIGMA].value() / 0.424661;
}

JsonObj Gaussian::saveJson() const {
  return super::saveJson().saveString(KEY_TYPE, KEY_GAUSSIAN);
}

//------------------------------------------------------------------------------

CauchyLorentz::CauchyLorentz(qreal ampl, qreal xShift, qreal gamma) {
  setParameterCount(3);

  auto &parAmpl   = parameters[parAMPL];
  auto &parXShift = parameters[parXSHIFT];
  auto &parGamma  = parameters[parGAMMA];

  parAmpl.setValueRange(0,qInf());
  parAmpl.setValue(ampl);

  parXShift.setValue(xShift);

  parGamma.setValueRange(0,qInf());
  parGamma.setValue(gamma);
}

qreal CauchyLorentz::y(qreal x, qreal const* parameterValues) const {
  qreal ampl   = parValue(parAMPL,   parameterValues);
  qreal xShift = parValue(parXSHIFT, parameterValues);
  qreal gamma  = parValue(parGAMMA,  parameterValues);

  qreal arg = (x - xShift) / gamma;
  return ampl / (1 + arg * arg);
}

qreal CauchyLorentz::dy(qreal x, uint parameterIndex, qreal const* parameterValues) const {
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

void CauchyLorentz::setGuessPeak(XY const& xy) {
  super::setGuessPeak(xy);
  setValue(parXSHIFT, xy.x);
  setValue(parAMPL,   xy.y);
}

void CauchyLorentz::setGuessFWHM(qreal val) {
  super::setGuessFWHM(val);
  // gamma = HWHM = FWHM / 2
  setValue(parGAMMA, val / 2);
}

XY CauchyLorentz::getFitPeak() const {
  return XY(parameters[parXSHIFT].value(),parameters[parAMPL].value());
}

qreal CauchyLorentz::getFitFWHM() const {
  return parameters[parGAMMA].value() * 2;
}

JsonObj CauchyLorentz::saveJson() const {
  return super::saveJson().saveString(KEY_TYPE, KEY_LORENTZIAN);
}

//------------------------------------------------------------------------------

PseudoVoigt1::PseudoVoigt1(qreal ampl, qreal xShift, qreal sigmaGamma, qreal eta) {
  setParameterCount(4);

  auto &parAmpl       = parameters[parAMPL];
  auto &parXShift     = parameters[parXSHIFT];
  auto &parSigmaGamma = parameters[parSIGMAGAMMA];
  auto &parEta        = parameters[parETA];

  parAmpl.setValueRange(0,qInf());
  parAmpl.setValue(ampl);

  parXShift.setValue(xShift);

  parSigmaGamma.setValueRange(0,qInf());
  parSigmaGamma.setValue(sigmaGamma);

  parEta.setValueRange(0,1);
  parEta.setValue(eta);
}

qreal PseudoVoigt1::y(qreal x, qreal const* parameterValues) const {
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

qreal PseudoVoigt1::dy(qreal x, uint parameterIndex, qreal const* parameterValues) const {
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

void PseudoVoigt1::setGuessPeak(XY const& xy) {
  super::setGuessPeak(xy);
  setValue(parXSHIFT, xy.x);
  setValue(parAMPL,   xy.y);
}

void PseudoVoigt1::setGuessFWHM(qreal val) {
  super::setGuessFWHM(val);
  setValue(parSIGMAGAMMA, val / 2);
}

XY PseudoVoigt1::getFitPeak() const {
  return XY(parameters[parXSHIFT].value(), parameters[parAMPL].value());
}

qreal PseudoVoigt1::getFitFWHM() const {
  return parameters[parSIGMAGAMMA].value() * 2;
}

JsonObj PseudoVoigt1::saveJson() const {
  return super::saveJson().saveString(KEY_TYPE, KEY_PSEUDOVOIGT1);
}

//------------------------------------------------------------------------------

PseudoVoigt2::PseudoVoigt2(qreal ampl, qreal mu, qreal hwhmG, qreal hwhmL, qreal eta) {
  setParameterCount(5);

  auto &parAmpl  = parameters[parAMPL];
  auto &parMu    = parameters[parXSHIFT];
  auto &parHwhmG = parameters[parSIGMA];
  auto &parHwhmL = parameters[parGAMMA];
  auto &parEta   = parameters[parETA];

  parAmpl.setValueRange(0,qInf());
  parAmpl.setValue(ampl);

  parMu.setValue(mu);

  parHwhmG.setValueRange(0,qInf());
  parHwhmG.setValue(hwhmG);

  parHwhmL.setValueRange(0,qInf());
  parHwhmL.setValue(hwhmL);

  parEta.setValueRange(0,1);
  parEta.setValue(eta);
}

qreal PseudoVoigt2::y(qreal x, qreal const* parameterValues) const {
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

qreal PseudoVoigt2::dy(qreal x, uint parameterIndex, qreal const* parameterValues) const {
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

void PseudoVoigt2::setGuessPeak(XY const& xy) {
  super::setGuessPeak(xy);
  setValue(parXSHIFT, xy.x);
  setValue(parAMPL,   xy.y);
}

void PseudoVoigt2::setGuessFWHM(qreal val) {
  super::setGuessFWHM(val);
  setValue(parSIGMA, val * 0.424661);
  setValue(parGAMMA, val / 2);
}

XY PseudoVoigt2::getFitPeak() const {
  return XY(parameters[parXSHIFT].value(), parameters[parAMPL].value());
}

qreal PseudoVoigt2::getFitFWHM() const {
  qreal eta = parameters[parETA].value();
  return
    ((1-eta) * parameters[parSIGMA].value() / 0.424661
     + eta * parameters[parGAMMA].value() * 2)
    / 2;
}

JsonObj PseudoVoigt2::saveJson() const {
  return super::saveJson().saveString(KEY_TYPE, KEY_PSEUDOVOIGT2);
}

//------------------------------------------------------------------------------
}}
// eof


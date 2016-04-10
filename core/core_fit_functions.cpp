// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_fit_functions.h
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
#include <qmath.h>

namespace core { namespace fit {
//------------------------------------------------------------------------------

Function::Parameter::Parameter()
  : _value(0), _range(Range::infinite())
  , _maxDelta(qQNaN()), _maxDeltaPercent(qQNaN())
  , _maxError(qQNaN()), _maxErrorPercent(qQNaN()) {
}

Range Function::Parameter::valueRange() const {
  return _range.isValid() ? _range : Range(_value);
}

void Function::Parameter::setValueRange(qreal min, qreal max) {
  _range.set(min,max);
}

bool Function::Parameter::checkConstraints(qreal value, qreal error) {
  if (_range.isValid() && !_range.contains(value))
    return false;

  if (!qIsNaN(_maxDelta) && qAbs(value - _value) > _maxDelta)
    return false;

  if (!qIsNaN(_maxDeltaPercent) &&
      (0 == _value || qAbs((value - _value) / _value) * 100 > _maxDeltaPercent))
    return false;

  if (!qIsNaN(_maxError) && error > _maxError)
    return false;

  if (!qIsNaN(_maxErrorPercent) &&
      (0 == _value || qAbs(error / _value) * 100 > _maxErrorPercent))
    return false;

  return true;
}

bool Function::Parameter::setValue(qreal value, qreal error, bool force) {
  if (!force && !checkConstraints(value,error))
    return false;

  _value = value;
  return true;
}

static str const
  KEY_VALUE("value"), KEY_RANGE("range"),
  KEY_MAX_DELTA("maxDelta"), KEY_MAX_DELTA_PERCENT("maxDeltaPercent"),
  KEY_MAX_ERROR("maxError"), KEY_MAX_ERROR_PERCENT("maxErrorPercent");

JsonObj Function::Parameter::saveJson() const {
  return JsonObj()
    .saveReal(KEY_VALUE,  _value)
    .saveRange(KEY_RANGE, _range)

    .saveReal(KEY_MAX_DELTA,         _maxDelta)
    .saveReal(KEY_MAX_DELTA_PERCENT, _maxDeltaPercent)
    .saveReal(KEY_MAX_ERROR,         _maxError)
    .saveReal(KEY_MAX_ERROR_PERCENT, _maxErrorPercent);
}

void Function::Parameter::loadJson(rcJsonObj obj) THROWS {
  _value   = obj.loadReal(KEY_VALUE);
  _range = obj.loadRange(KEY_RANGE);

  _maxDelta        = obj.loadReal(KEY_MAX_DELTA);
  _maxDeltaPercent = obj.loadReal(KEY_MAX_DELTA_PERCENT);
  _maxError        = obj.loadReal(KEY_MAX_ERROR);
  _maxErrorPercent = obj.loadReal(KEY_MAX_ERROR_PERCENT);
}

//------------------------------------------------------------------------------

static str const
  KEY_FUNCTION_TYPE("type"),

  KEY_SUM_FUNCTIONS("sum"),
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

Function* Function::factory(rcJsonObj obj) THROWS {
  QScopedPointer<Function> f(factory(obj.loadString(KEY_FUNCTION_TYPE)));
  f->loadJson(obj);
  return f.take();
}

Function::Function() {
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
  _parameters.fill(Parameter(),count);
}

uint SimpleFunction::parameterCount() const {
  return _parameters.count();
}

Function::Parameter& SimpleFunction::parameterAt(uint i) {
  return _parameters[i];
}

void SimpleFunction::reset() {
  for_i (_parameters.count()) {
    auto &p = _parameters[i];
    p.setValue(p.valueRange().bound(0));
  }
}

static str const KEY_PARAMS("parameters");

JsonObj SimpleFunction::saveJson() const {
  JsonArr params;

  for (auto &param: _parameters)
    params.append(param.saveJson());

  return super::saveJson() + JsonObj().saveArr(KEY_PARAMS,params);
}

void SimpleFunction::loadJson(rcJsonObj obj) THROWS {
  super::loadJson(obj);

  JsonArr params = obj.loadArr(KEY_PARAMS);

  uint parCount = params.count();
  setParameterCount(parCount);

  for_i (parCount)
    _parameters[i].loadJson(params.objAt(i));
}

qreal SimpleFunction::parValue(uint i, qreal const* parValues) const {
  return parValues ? parValues[i] : _parameters[i].value();
}

void SimpleFunction::setValue(uint i, qreal val) {
  _parameters[i].setValue(val);
}

//------------------------------------------------------------------------------

SumFunctions::SumFunctions() {
}

SumFunctions::~SumFunctions() {
  // dispose of the Functions that were added
  for (Function *f: _functions) delete f;
}

void SumFunctions::addFunction(Function* function) {
  ASSERT(function)

  uint parIndex = parameterCount();
  _functions.append(function);

  for_i (function->parameterCount()) {
    // aggregate parameter list
    _allParameters.append(&function->parameterAt(i));
    // lookup helpers
    _function4parIndex.append(function);
    _firstParIndex4parIndex.append(parIndex);
  }
}

uint SumFunctions::parameterCount() const {
  return _allParameters.count();
}

Function::Parameter& SumFunctions::parameterAt(uint i) {
  return *_allParameters.at(i);
}

qreal SumFunctions::y(qreal x, qreal const* parValues) const {
  qreal sum = 0;

  for (Function *f: _functions) {
    sum += f->y(x,parValues);

    if (parValues)
      parValues += f->parameterCount(); // advance to next function
  }

  return sum;
}

qreal SumFunctions::dy(qreal x, uint parIndex, qreal const* parValues) const {
  Function *f = _function4parIndex.at(parIndex); // aggregate parIndex refers to function f

  // offset parameter index
  uint firstIndex = _firstParIndex4parIndex[parIndex];
  if (parValues) parValues += firstIndex;

  ASSERT(firstIndex <= parIndex)
  parIndex -= firstIndex;

  ASSERT(parIndex < f->parameterCount())
  return f->dy(x, parIndex, parValues);
}

static str const KEY_FUNCTION_COUNT("function count"), KEY_FUNCTION("f%1");

JsonObj SumFunctions::saveJson() const {
  JsonObj obj;
  obj.saveString(KEY_FUNCTION_TYPE, KEY_SUM_FUNCTIONS);

  uint funCount = _functions.count();
  obj.saveUint(KEY_FUNCTION_COUNT,funCount);

  for_i (funCount)
    obj.saveObj(KEY_FUNCTION.arg(i+1), _functions[i]->saveJson());

  return super::saveJson() + obj;
}

void SumFunctions::loadJson(rcJsonObj obj) THROWS {
  RUNTIME_CHECK(_functions.isEmpty(), "non-empty sum of functions; cannot load twice");

  super::loadJson(obj);

  uint funCount = obj.loadUint(KEY_FUNCTION_COUNT);

  for_i (funCount) {
    auto funObj = obj.loadObj(KEY_FUNCTION.arg(i+1));
    QScopedPointer<Function> f(Function::factory(funObj.loadString(KEY_FUNCTION_TYPE)));
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

qreal Polynomial::y(qreal x, qreal const* parValues) const {
  qreal val = 0, xPow = 1;
  for_i (_parameters.count()) {
    val += parValue(i,parValues) * xPow;
    xPow *= x;
  }
  return val;
}

qreal Polynomial::dy(qreal x, uint i, qreal const*) const {
  return pow_n(x,i);
}

// REVIEW
qreal Polynomial::avgY(rcRange rgeX, qreal const* parValues) const {
  ASSERT (rgeX.isValid())

  qreal w = rgeX.width();
  if (w <= 0) return y(rgeX.min, parValues);

  qreal minY = 0, maxY = 0, minPow = 1, maxPow = 1;

  for_i (_parameters.count()) {
    qreal facY = parValue(i,parValues) / (i+1);
    minY += facY * (minPow *= rgeX.min);
    maxY += facY * (maxPow *= rgeX.max);
  }

  return (1/w) * (maxY-minY);
}

JsonObj Polynomial::saveJson() const {
  JsonObj obj;
  obj.saveString(KEY_FUNCTION_TYPE, KEY_POLYNOMIAL);
  return super::saveJson() + obj;
}

void Polynomial::loadJson(rcJsonObj obj) THROWS {
  super::loadJson(obj);
}

//------------------------------------------------------------------------------

PeakFunction* PeakFunction::factory(ePeakType type) {
  switch (type) {
  case ePeakType::GAUSSIAN:     return new Gaussian();
  case ePeakType::LORENTZIAN:   return new CauchyLorentz();
  case ePeakType::PSEUDOVOIGT1: return new PseudoVoigt1();
  case ePeakType::PSEUDOVOIGT2: return new PseudoVoigt2();
  default:
    NEVER_HERE
    return nullptr;
  }
}

PeakFunction* PeakFunction::clone() const {
  PeakFunction *f = factory(type());
  *f = *this; // REVIEW
  return f;
}

PeakFunction::PeakFunction(): _guessedPeak(), _guessedFWHM(qQNaN()) {
}

void PeakFunction::setGuessedPeak(rcXY peak) {
  _guessedPeak = peak;
}

void PeakFunction::setGuessedFWHM(qreal fwhm) {
  _guessedFWHM = fwhm;
}

void PeakFunction::reset() {
  super::reset();
  setGuessedPeak(_guessedPeak);
  setGuessedFWHM(_guessedFWHM);
}

static str const KEY_GUESSED_PEAK("guessed peak"), KEY_GUESSED_FWHM("guessed fwhm");

JsonObj PeakFunction::saveJson() const {
  return super::saveJson()
      .saveObj(KEY_GUESSED_PEAK,_guessedPeak.saveJson())
      .saveReal(KEY_GUESSED_FWHM,_guessedFWHM);
}

void PeakFunction::loadJson(rcJsonObj obj) THROWS {
  super::loadJson(obj);
  _guessedPeak.loadJson(obj.loadObj(KEY_GUESSED_PEAK));
  _guessedFWHM = obj.loadReal(KEY_GUESSED_FWHM);
}

//------------------------------------------------------------------------------

Gaussian::Gaussian(qreal ampl, qreal xShift, qreal sigma) {
  setParameterCount(3);

  auto &parAmpl   = _parameters[parAMPL];
  auto &parXShift = _parameters[parXSHIFT];
  auto &parSigma  = _parameters[parSIGMA];

  parAmpl.setValueRange(0,qInf());
  parAmpl.setValue(ampl);

  parXShift.setValue(xShift);

  parSigma.setValueRange(0,qInf());
  parSigma.setValue(sigma);
}

qreal Gaussian::y(qreal x, qreal const* parValues) const {
  qreal ampl   = parValue(parAMPL,   parValues);
  qreal xShift = parValue(parXSHIFT, parValues);
  qreal sigma  = parValue(parSIGMA,  parValues);

  qreal arg = (x - xShift) / sigma;
  qreal exa = exp(-0.5 * arg * arg);

  return ampl * exa;
}

qreal Gaussian::dy(qreal x, uint parIndex, qreal const* parValues) const {
  qreal ampl   = parValue(parAMPL,   parValues);
  qreal xShift = parValue(parXSHIFT, parValues);
  qreal sigma  = parValue(parSIGMA,  parValues);

  qreal arg = (x - xShift) / sigma;
  qreal exa = exp(-0.5 * arg * arg);

  switch (parIndex) {
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

void Gaussian::setGuessedPeak(rcXY xy) {
  super::setGuessedPeak(xy);
  setValue(parXSHIFT, xy.x);
  setValue(parAMPL,   xy.y);
}

void Gaussian::setGuessedFWHM(qreal val) {
  super::setGuessedFWHM(val);
  // sigma = FWHM * 1/4 * (SQRT(2)/SQRT(ln(2))) = FWHM * 0.424661
  setValue(parSIGMA, val * 0.424661);
}

XY Gaussian::fittedPeak() const {
  return XY(_parameters[parXSHIFT].value(), _parameters[parAMPL].value());
}

qreal Gaussian::fittedFWHM() const {
  return _parameters[parSIGMA].value() / 0.424661;
}

JsonObj Gaussian::saveJson() const {
  return super::saveJson()
      .saveString(KEY_FUNCTION_TYPE, KEY_GAUSSIAN);
}

//------------------------------------------------------------------------------

CauchyLorentz::CauchyLorentz(qreal ampl, qreal xShift, qreal gamma) {
  setParameterCount(3);

  auto &parAmpl   = _parameters[parAMPL];
  auto &parXShift = _parameters[parXSHIFT];
  auto &parGamma  = _parameters[parGAMMA];

  parAmpl.setValueRange(0,qInf());
  parAmpl.setValue(ampl);

  parXShift.setValue(xShift);

  parGamma.setValueRange(0,qInf());
  parGamma.setValue(gamma);
}

qreal CauchyLorentz::y(qreal x, qreal const* parValues) const {
  qreal ampl   = parValue(parAMPL,   parValues);
  qreal xShift = parValue(parXSHIFT, parValues);
  qreal gamma  = parValue(parGAMMA,  parValues);

  qreal arg = (x - xShift) / gamma;
  return ampl / (1 + arg * arg);
}

qreal CauchyLorentz::dy(qreal x, uint parIndex, qreal const* parValues) const {
  qreal ampl   = parValue(parAMPL,   parValues);
  qreal xShift = parValue(parXSHIFT, parValues);
  qreal gamma  = parValue(parGAMMA,  parValues);

  qreal arg1 = (x - xShift) / gamma;
  qreal arg2 = arg1 * arg1;
  qreal arg3 = (1 + arg2) * (1 + arg2);

  switch (parIndex) {
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

void CauchyLorentz::setGuessedPeak(rcXY xy) {
  super::setGuessedPeak(xy);
  setValue(parXSHIFT, xy.x);
  setValue(parAMPL,   xy.y);
}

void CauchyLorentz::setGuessedFWHM(qreal val) {
  super::setGuessedFWHM(val);
  // gamma = HWHM = FWHM / 2
  setValue(parGAMMA, val / 2);
}

XY CauchyLorentz::fittedPeak() const {
  return XY(_parameters[parXSHIFT].value(),_parameters[parAMPL].value());
}

qreal CauchyLorentz::fittedFWHM() const {
  return _parameters[parGAMMA].value() * 2;
}

JsonObj CauchyLorentz::saveJson() const {
  return super::saveJson()
      .saveString(KEY_FUNCTION_TYPE, KEY_LORENTZIAN);
}

//------------------------------------------------------------------------------

PseudoVoigt1::PseudoVoigt1(qreal ampl, qreal xShift, qreal sigmaGamma, qreal eta) {
  setParameterCount(4);

  auto &parAmpl       = _parameters[parAMPL];
  auto &parXShift     = _parameters[parXSHIFT];
  auto &parSigmaGamma = _parameters[parSIGMAGAMMA];
  auto &parEta        = _parameters[parETA];

  parAmpl.setValueRange(0,qInf());
  parAmpl.setValue(ampl);

  parXShift.setValue(xShift);

  parSigmaGamma.setValueRange(0,qInf());
  parSigmaGamma.setValue(sigmaGamma);

  parEta.setValueRange(0,1);
  parEta.setValue(eta);
}

qreal PseudoVoigt1::y(qreal x, qreal const* parValues) const {
  qreal ampl       = parValue(parAMPL,       parValues);
  qreal xShift     = parValue(parXSHIFT,     parValues);
  qreal sigmaGamma = parValue(parSIGMAGAMMA, parValues);
  qreal eta        = parValue(parETA,        parValues);

  qreal arg      = (x - xShift) / sigmaGamma;
  qreal arg2     = arg * arg;
  qreal gaussian = ampl * exp(-arg2 * log(2.0));
  qreal lorentz  = ampl / (1 + arg2);

  return (1-eta) * gaussian + eta * lorentz;
}

qreal PseudoVoigt1::dy(qreal x, uint parIndex, qreal const* parValues) const {
  qreal ampl       = parValue(parAMPL,       parValues);
  qreal xShift     = parValue(parXSHIFT,     parValues);
  qreal sigmaGamma = parValue(parSIGMAGAMMA, parValues);
  qreal eta        = parValue(parETA,        parValues);

  qreal arg1 = (x - xShift) / sigmaGamma;
  qreal arg2 = arg1 * arg1;
  qreal arg3 = exp(-arg2 * log(2.0));
  qreal arg4 = 1 + arg2;

  switch (parIndex) {
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

void PseudoVoigt1::setGuessedPeak(rcXY xy) {
  super::setGuessedPeak(xy);
  setValue(parXSHIFT, xy.x);
  setValue(parAMPL,   xy.y);
}

void PseudoVoigt1::setGuessedFWHM(qreal val) {
  super::setGuessedFWHM(val);
  setValue(parSIGMAGAMMA, val / 2);
}

XY PseudoVoigt1::fittedPeak() const {
  return XY(_parameters[parXSHIFT].value(), _parameters[parAMPL].value());
}

qreal PseudoVoigt1::fittedFWHM() const {
  return _parameters[parSIGMAGAMMA].value() * 2;
}

JsonObj PseudoVoigt1::saveJson() const {
  return super::saveJson()
      .saveString(KEY_FUNCTION_TYPE, KEY_PSEUDOVOIGT1);
}

//------------------------------------------------------------------------------

PseudoVoigt2::PseudoVoigt2(qreal ampl, qreal mu, qreal hwhmG, qreal hwhmL, qreal eta) {
  setParameterCount(5);

  auto &parAmpl  = _parameters[parAMPL];
  auto &parMu    = _parameters[parXSHIFT];
  auto &parHwhmG = _parameters[parSIGMA];
  auto &parHwhmL = _parameters[parGAMMA];
  auto &parEta   = _parameters[parETA];

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

qreal PseudoVoigt2::y(qreal x, qreal const* parValues) const {
  qreal ampl   = parValue(parAMPL,   parValues);
  qreal xShift = parValue(parXSHIFT, parValues);
  qreal sigma  = parValue(parSIGMA,  parValues);
  qreal gamma  = parValue(parGAMMA,  parValues);
  qreal eta    = parValue(parETA,    parValues);

  qreal argG     = (x - xShift) / sigma;
  qreal argG2    = argG * argG;
  qreal gaussian = ampl * exp(-argG2 * log(2.0));

  qreal argL     = (x - xShift) / gamma;
  qreal argL2    = argL * argL;
  qreal lorentz  = ampl / (1 + argL2);

  return (1-eta) * gaussian + eta * lorentz;
}

qreal PseudoVoigt2::dy(qreal x, uint parIndex, qreal const* parValues) const {
  qreal ampl   = parValue(parAMPL,   parValues);
  qreal xShift = parValue(parXSHIFT, parValues);
  qreal sigma  = parValue(parSIGMA,  parValues);
  qreal gamma  = parValue(parGAMMA,  parValues);
  qreal eta    = parValue(parETA,    parValues);

  qreal argG1 = (x - xShift) / sigma;
  qreal argG2 = argG1 * argG1;
  qreal argG3 = exp(-argG2 * log(2.0));

  qreal argL1 = (x - xShift) / gamma;
  qreal argL2 = argL1 * argL1;
  qreal argL3 = 1 + argL2;

  switch (parIndex) {
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

void PseudoVoigt2::setGuessedPeak(rcXY xy) {
  super::setGuessedPeak(xy);
  setValue(parXSHIFT, xy.x);
  setValue(parAMPL,   xy.y);
}

void PseudoVoigt2::setGuessedFWHM(qreal val) {
  super::setGuessedFWHM(val);
  setValue(parSIGMA, val * 0.424661);
  setValue(parGAMMA, val / 2);
}

XY PseudoVoigt2::fittedPeak() const {
  return XY(_parameters[parXSHIFT].value(), _parameters[parAMPL].value());
}

qreal PseudoVoigt2::fittedFWHM() const {
  qreal eta = _parameters[parETA].value();
  return
    ((1-eta) * _parameters[parSIGMA].value() / 0.424661
     + eta * _parameters[parGAMMA].value() * 2)
    / 2;
}

JsonObj PseudoVoigt2::saveJson() const {
  return super::saveJson()
      .saveString(KEY_FUNCTION_TYPE, KEY_PSEUDOVOIGT2);
}

//------------------------------------------------------------------------------
}}
// eof


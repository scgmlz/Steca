#include "core_fit_functions.h"
#include <QJsonObject>
#include <QScopedPointer>
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

static str KEY_VALUE("value");
static str KEY_RANGE("range");
static str KEY_MAX_DELTA("maxDelta");
static str KEY_MAX_DELTA_PERCENT("maxDeltaPercent");
static str KEY_MAX_ERROR("maxError");
static str KEY_MAX_ERROR_PERCENT("maxErrorPercent");
static str KEY_PARAMETERS("parameters");
static str KEY_FUNCTIONS("functions");
static str KEY_FUNCTION("f%1");
static str KEY_FUNCTIONS_COUNT("functionsCount");
static str KEY_SUM_FUNCTIONS("SumFunctions");
static str KEY_TYPE("type");
static str KEY_POLYNOMIAL("polynomial");
static str KEY_GAUSSIAN("Gaussian");
static str KEY_LORENTZIAN("CauchyLorentz");
static str KEY_PSEUDOVOIGT1("PseudoVoigt1");
static str KEY_PSEUDOVOIGT2("PseudoVoigt2");
static str KEY_GUESS_FWHM("guess_fwhm");

void Function::Parameter::loadFrom(QJsonObject const& obj) THROWS {
  value = loadReal(obj,KEY_VALUE);
  range.loadFrom(obj[KEY_RANGE].toObject());

  maxDelta        = loadReal(obj,KEY_MAX_DELTA);
  maxDeltaPercent = loadReal(obj,KEY_MAX_DELTA_PERCENT);
  maxError        = loadReal(obj,KEY_MAX_ERROR);
  maxErrorPercent = loadReal(obj,KEY_MAX_ERROR_PERCENT);
}

void Function::Parameter::saveTo(QJsonObject& obj) const {
  saveReal(obj,KEY_VALUE, value);

  QJsonObject rgeObj;
  range.saveTo(rgeObj);
  obj[KEY_RANGE] = rgeObj;

  saveReal(obj,KEY_MAX_DELTA,         maxDelta);
  saveReal(obj,KEY_MAX_DELTA_PERCENT, maxDeltaPercent);
  saveReal(obj,KEY_MAX_ERROR,         maxError);
  saveReal(obj,KEY_MAX_ERROR_PERCENT, maxErrorPercent);
}

//------------------------------------------------------------------------------

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
}

Function::Function() {
}

Function::~Function() {
}

static str KEY_PAR_COUNT("parCount");
static str KEY_PARAMETER("p%1");

void Function::loadFrom(QJsonObject const&) THROWS {
  // nothing to see here; move along
}

void Function::saveTo(QJsonObject&) const {
  // nothing to do
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
  ASSERT(count > 0)
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


void SimpleFunction::loadFrom(QJsonObject const& obj) THROWS {
  super::loadFrom(obj);

  int parCount = obj[KEY_PAR_COUNT].toInt();
  RUNTIME_CHECK(parCount > 0,"parameter count not valid");
  setParameterCount(parCount);
  for_i (parCount) {
    parameters[i].loadFrom(obj[str(KEY_PARAMETER).arg(i+1)].toObject());
  }
}

void SimpleFunction::saveTo(QJsonObject& obj) const {
  super::saveTo(obj);

  auto parCount = parameters.count();
  obj[KEY_PAR_COUNT] = parCount;
  for_i (parCount) {
    QJsonObject pObj;
    parameters[i].saveTo(pObj);
    obj[str(KEY_PARAMETER).arg(i+1)] = pObj;
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

void SumFunctions::loadFrom(QJsonObject const& obj) THROWS {
  RUNTIME_CHECK(functions.isEmpty(),"non-empty sum of functions; cannot load twice");

  super::loadFrom(obj);

  int funCount = obj[KEY_FUNCTIONS_COUNT].toInt();
  RUNTIME_CHECK(funCount >= 0,"function count not valid");

  for_i (funCount) {
    auto fObj = obj[str(KEY_FUNCTION).arg(i+1)].toObject();
    QScopedPointer<Function> f(Function::factory(fObj[KEY_TYPE].toString()));
    f->loadFrom(fObj);
    addFunction(f.take());
  }
}

void SumFunctions::saveTo(QJsonObject& obj) const {
  super::saveTo(obj);

  obj[KEY_TYPE] = KEY_SUM_FUNCTIONS;

  auto funCount = functions.count();
  obj[KEY_FUNCTIONS_COUNT] = funCount;
  for_i (funCount) {
    QJsonObject pObj;
    functions[i]->saveTo(pObj);
    obj[str(KEY_FUNCTION).arg(i+1)] = pObj;
  }
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

void Polynomial::loadFrom(QJsonObject const& obj) THROWS {
  super::loadFrom(obj);
}

void Polynomial::saveTo(QJsonObject& obj) const {
  super::saveTo(obj);
  obj[KEY_TYPE] = KEY_POLYNOMIAL;
}

//------------------------------------------------------------------------------

PeakFunction *PeakFunction::factory(eType type) {
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
  }
}

PeakFunction::PeakFunction(): guessPeak(), guessFwhm(qQNaN()) {
}

void PeakFunction::setGuessPeak(XY const& peak_) {
  guessPeak = peak_;
}

void PeakFunction::setGuessFWHM(qreal fwhm_) {
  guessFwhm = fwhm_;
}

void PeakFunction::reset() {
  super::reset();
  setGuessPeak(getGuessPeak());
  setGuessFWHM(getGuessFWHM());
}

void PeakFunction::loadFrom(QJsonObject const& obj) THROWS {
  super::loadFrom(obj);
  guessPeak.loadFrom(obj);
  guessFwhm = loadReal(obj,KEY_GUESS_FWHM);
}

void PeakFunction::saveTo(QJsonObject& obj) const {
  super::saveTo(obj);
  guessPeak.saveTo(obj);
  saveReal(obj,KEY_GUESS_FWHM,guessFwhm);
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

XY Gaussian::getFitPeak() {
  return XY(parameters[parXSHIFT].getValue(),parameters[parAMPL].getValue());
}

qreal Gaussian::getFitFWHM() {
  return parameters[parSIGMA].getValue() / 0.424661;
}

void Gaussian::saveTo(QJsonObject& obj) const {
  super::saveTo(obj);
  obj[KEY_TYPE] = KEY_GAUSSIAN;
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

XY CauchyLorentz::getFitPeak() {
  return XY(parameters[parXSHIFT].getValue(),parameters[parAMPL].getValue());
}

qreal CauchyLorentz::getFitFWHM() {
  return parameters[parGAMMA].getValue() * 2;
}

void CauchyLorentz::saveTo(QJsonObject& obj) const {
  super::saveTo(obj);
  obj[KEY_TYPE] = KEY_LORENTZIAN;
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

void PseudoVoigt1::setGuessPeak(XY const& xy) {
  super::setGuessPeak(xy);
  setValue(parXSHIFT, xy.x);
  setValue(parAMPL,   xy.y);
}

void PseudoVoigt1::setGuessFWHM(qreal val) {
  super::setGuessFWHM(val);
  setValue(parSIGMAGAMMA, val / 2);
}

XY PseudoVoigt1::getFitPeak() {
  return XY(parameters[parXSHIFT].getValue(),parameters[parAMPL].getValue());
}

qreal PseudoVoigt1::getFitFWHM() {
  return parameters[parSIGMAGAMMA].getValue() * 2;
}

void PseudoVoigt1::saveTo(QJsonObject& obj) const {
  super::saveTo(obj);
  obj[KEY_TYPE] = KEY_PSEUDOVOIGT1;
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

XY PseudoVoigt2::getFitPeak() {
  return XY(parameters[parXSHIFT].getValue(),parameters[parAMPL].getValue());
}

qreal PseudoVoigt2::getFitFWHM() {
  qreal eta = parameters[parETA].getValue();
  return
    ((1-eta) * parameters[parSIGMA].getValue() / 0.424661
     + eta * parameters[parGAMMA].getValue() * 2)
    / 2;
}

void PseudoVoigt2::saveTo(QJsonObject& obj) const {
  super::saveTo(obj);
  obj[KEY_TYPE] = KEY_PSEUDOVOIGT2;
}

//------------------------------------------------------------------------------
}}
// eof

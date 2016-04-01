#include "core_reflection.h"
#include "types/core_json.h"
#include <QStringList>

namespace core {

//------------------------------------------------------------------------------

str_lst const& Reflection::reflTypes() {
  static str_lst types;
  if (types.isEmpty())
    types << "Gaussian" << "Lorentzian" << "PseudoVoigt1" << "PseudoVoigt2";
  return types;
}

Reflection::Reflection(eType type): peakFunction(nullptr) {
  setPeakFunction(type);
  setRange(Range());
}

Reflection::~Reflection() {
  delete peakFunction;
}

Reflection::eType Reflection::getType() const {
  return peakFunction->type();
}

void Reflection::setType(eType type) {
  setPeakFunction(type);
}

void Reflection::setRange(Range const& range_) {
  range = range_;
}

fit::PeakFunction* Reflection::makePeakFunction() const {
  QScopedPointer<fit::PeakFunction> f(fit::PeakFunction::factory(getType()));
  f->setGuessPeak(peakFunction->getGuessPeak());
  f->setGuessFWHM(peakFunction->getGuessFWHM());
  return f.take();
}

fit::PeakFunction& Reflection::getPeakFunction() {
  ASSERT(peakFunction)
  return *peakFunction;
}

fit::PeakFunction const& Reflection::getPeakFunction() const {
  ASSERT(peakFunction)
  return *peakFunction;
}

void Reflection::invalidateGuesses() {
  peakFunction->setGuessPeak(XY());
  peakFunction->setGuessFWHM(qQNaN());
}

void Reflection::setPeakFunction(eType type) {
  setPeakFunction(fit::PeakFunction::factory(type));
}

void Reflection::setPeakFunction(fit::PeakFunction* f) {
  delete peakFunction;
  peakFunction = f;
}

static str const KEY_FWHM("fwhm");
static str const KEY_TYPE("type");
static str const KEY_RANGE("range");
static str const KEY_PEAK("peak");

void Reflection::loadJson(rcJsonObj obj) THROWS {
  range.loadJson(obj[KEY_RANGE].toObject());

  JsonObj pObj = obj[KEY_PEAK].toObject();
  QScopedPointer<fit::Function> f(fit::Function::factory(pObj[KEY_TYPE].toString()));
  f->loadJson(pObj);

  RUNTIME_CHECK(dynamic_cast<fit::PeakFunction*>(f.data()),"must be a peak function");
  setPeakFunction(static_cast<fit::PeakFunction*>(f.take()));
}

JsonObj Reflection::saveJson() const {
  JsonObj obj;
  obj[KEY_RANGE] = getRange().saveJson();
  obj[KEY_PEAK]  = peakFunction->saveJson();
  return obj;
}

//------------------------------------------------------------------------------
}
// eof

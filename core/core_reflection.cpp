#include "core_reflection.h"
#include "core_json.h"
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

std::unique_ptr<fit::PeakFunction> Reflection::makePeakFunction() const {
  std::unique_ptr<fit::PeakFunction> f(fit::PeakFunction::factory(getType()));
  f->setGuessPeak(peakFunction->getGuessPeak());
  f->setGuessFWHM(peakFunction->getGuessFWHM());
  return f;
}

Reflection::PeakFunction& Reflection::getPeakFunction() {
  ASSERT(peakFunction)
  return *peakFunction;
}

Reflection::PeakFunction const& Reflection::getPeakFunction() const {
  ASSERT(peakFunction)
  return *peakFunction;
}

void Reflection::invalidateGuesses() {
  peakFunction->setGuessPeak(XY());
  peakFunction->setGuessFWHM(qQNaN());
}

void Reflection::setPeakFunction(eType type) {
  setPeakFunction(PeakFunction::factory(type));
}

void Reflection::setPeakFunction(PeakFunction* f) {
  delete peakFunction;
  peakFunction = f;
}

static str KEY_FWHM("fwhm");
static str KEY_TYPE("type");
static str KEY_RANGE("range");
static str KEY_PEAK("peak");

void Reflection::loadJson(rcJsonObj obj) THROWS {
  range.loadJson(obj[KEY_RANGE].toObject());

  JsonObj pObj = obj[KEY_PEAK].toObject();
  QScopedPointer<fit::Function> f(fit::Function::factory(pObj[KEY_TYPE].toString()));
  f->loadJson(pObj);

  RUNTIME_CHECK(dynamic_cast<PeakFunction*>(f.data()),"must be a peak function");
  setPeakFunction(static_cast<PeakFunction*>(f.take()));
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

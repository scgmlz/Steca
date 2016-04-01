#include "core_reflection.h"
#include "types/core_json.h"
#include <QStringList>

namespace core {

//------------------------------------------------------------------------------

str_lst const& Reflection::reflTypes() {
  static str_lst types {"Gaussian","Lorentzian","PseudoVoigt1","PseudoVoigt2"};
  return types;
}

rcstr Reflection::reflType(ePeakType type) {
  return reflTypes()[(int)type];
}

Reflection::Reflection(ePeakType type): peakFunction(nullptr) {
  setPeakFunction(type);
  setRange(Range());
}

Reflection::~Reflection() {
  delete peakFunction;
}

ePeakType Reflection::getType() const {
  return peakFunction->type();
}

void Reflection::setType(ePeakType type) {
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

void Reflection::setPeakFunction(ePeakType type) {
  setPeakFunction(fit::PeakFunction::factory(type));
}

void Reflection::setPeakFunction(fit::PeakFunction* f) {
  delete peakFunction;
  peakFunction = f;
}

static str const
  KEY_FWHM("fwhm"), KEY_TYPE("type"), KEY_RANGE("range"), KEY_PEAK("peak");

JsonObj Reflection::saveJson() const {
  return JsonObj()
    .saveRange(KEY_RANGE, getRange())
    .saveObj(KEY_PEAK, peakFunction->saveJson());
}

void Reflection::loadJson(rcJsonObj obj) THROWS {
  range = obj.loadRange(KEY_RANGE);

  JsonObj peakObj = obj.loadObj(KEY_PEAK);
  QScopedPointer<fit::Function> f(fit::Function::factory(peakObj.loadString(KEY_TYPE)));
  f->loadJson(peakObj);

  RUNTIME_CHECK(dynamic_cast<fit::PeakFunction*>(f.data()),"must be a peak function");
  setPeakFunction(static_cast<fit::PeakFunction*>(f.take()));
}

//------------------------------------------------------------------------------
}
// eof

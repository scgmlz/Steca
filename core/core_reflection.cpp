#include "core_reflection.h"
#include "core_types.h"
#include <QStringList>
#include <QJsonObject>

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

Reflection::PeakFunction &Reflection::getPeakFunction() const {
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

void Reflection::loadFrom(QJsonObject const& obj) THROWS {
  range.loadFrom(obj[KEY_RANGE].toObject());

  QJsonObject pObj = obj[KEY_PEAK].toObject();
  QScopedPointer<fit::Function> f(fit::Function::factory(pObj[KEY_TYPE].toString()));
  f->loadFrom(pObj);

  RUNTIME_CHECK(dynamic_cast<PeakFunction*>(f.data()),"must be a peak function");
  setPeakFunction(static_cast<PeakFunction*>(f.take()));
}

void Reflection::saveTo(QJsonObject& obj) const {
  QJsonObject rObj;
  getRange().saveTo(rObj);
  obj[KEY_RANGE] = rObj;

  QJsonObject pObj;
  peakFunction->saveTo(pObj);
  obj[KEY_PEAK] = pObj;
}

//------------------------------------------------------------------------------
}
// eof

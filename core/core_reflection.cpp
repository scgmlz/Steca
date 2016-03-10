#include "core_reflection.h"
#include <QStringList>

namespace core {

//------------------------------------------------------------------------------

str_lst const& Reflection::reflTypes() {
  static str_lst types;
  if (types.isEmpty())
    types << "Gaussian" << "Lorentzian" << "PseudoVoigt1" << "PseudoVoigt2";
  return types;
}

Reflection::Reflection(eType type): peakFunction(nullptr) /*>>>type(type_)*/ {
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
  // invalidate guesses
  peakFunction->setGuessPeak(XY());
  peakFunction->setGuessFWHM(qQNaN());
}

Reflection::PeakFunction &Reflection::getPeakFunction() const {
  ASSERT(peakFunction)
  return *peakFunction;
}

void Reflection::setPeakFunction(eType type) {
  delete peakFunction;
  peakFunction = PeakFunction::factory(type);
}

//------------------------------------------------------------------------------
}
// eof

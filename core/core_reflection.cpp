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

Reflection::Reflection(eType type_): type(type_) {
  setRange(Range());
}

Reflection::eType Reflection::getType() const {
  return type;
}

void Reflection::setType(Reflection::eType type_) {
  type = type_;
}

void Reflection::setRange(Range const& range_) {
  range = range_;
  // invalidate guesses
  peak  = XY();
  fwhm  = qQNaN();
}

fit::PeakFunction* Reflection::peakFunction() const {
  switch (type) {
  case REFL_GAUSSIAN:
    return new fit::Gaussian();
  case REFL_LORENTZIAN:
    return new fit::CauchyLorentz();
  case REFL_PSEUDOVOIGT1:
    return new fit::PseudoVoigt1();
  case REFL_PSEUDOVOIGT2:
    return new fit::PseudoVoigt2();
  }
}

//------------------------------------------------------------------------------
}
// eof

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
}

Reflection::eType Reflection::getType() const {
  return type;
}

void Reflection::setType(Reflection::eType type_) {
  type = type_;
}

Range const& Reflection::getRange() const {
  return range;
}

void Reflection::setRange(Range const& range_) {
  range = range_;
}

//------------------------------------------------------------------------------
}
// eof

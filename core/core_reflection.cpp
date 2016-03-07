#include "core_reflection.h"

namespace core {

//------------------------------------------------------------------------------

str_lst const& Reflection::peakTypes() {
  static str_lst types;
  if (types.isEmpty())
    types << "Integral" << "Gaussian" << "Lorentzian" << "PseudoVoigt1" << "PseudoVoigt2";
  return types;
}

//------------------------------------------------------------------------------
}
// eof

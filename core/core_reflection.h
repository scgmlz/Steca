/** \file
 * Detector images.
 */

#ifndef CORE_REFLECTION_H
#define CORE_REFLECTION_H

#include "core_types.h"
#include "core_array2d.h"

namespace core {
//------------------------------------------------------------------------------

class Reflection {
public:
  enum eType {
    REFL_GAUSSIAN, REFL_LORENTZIAN, REFL_PSEUDOVOIGT1, REFL_PSEUDOVOIGT2,
  };

  static str_lst const& reflTypes();

  Reflection(eType = REFL_GAUSSIAN);

  eType getType() const;
  void  setType(eType);

private:
  eType type;
  core::Range range;
};

typedef QVector<Reflection> Reflections;

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::Reflection*)

#endif

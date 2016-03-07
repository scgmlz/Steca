/** \file
 * Detector images.
 */

#ifndef CORE_REFLECTION_H
#define CORE_REFLECTION_H

#include "core_defs.h"
#include "core_array2d.h"

namespace core {
//------------------------------------------------------------------------------

class Reflection {
public:
  enum {
    PEAK_INTEGRAL, PEAK_GAUSSIAN, PEAK_LORENTZIAN, PEAK_PSEUDOVOIGT1, PEAK_PSEUDOVOIGT2,
  };

  static str_lst const& peakTypes();
};

//------------------------------------------------------------------------------
}
#endif

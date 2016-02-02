/** \file
 * Core-supporting types and functions.
 */

#include "core_defs.h"

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

namespace core {

///< closed interval
struct Interval {
  Interval(qreal val = 0);
  Interval(qreal min, qreal max);

  qreal min, max;

  void set(qreal val);            ///< both min and max = val
  void set(qreal min,qreal max);  ///< must be: min<=max
  void safeSet(qreal,qreal);      ///< will be set in the right order min/max

  void extend(qreal val);         ///< extend the interval to include val
  bool contains(qreal val) const;
};

// TODO see if all three*two are needed, disassemble?
struct Borders {
  Interval
    gamma,
    tth_regular,
    tth_gamma0; // at gamma=0
};

// conversion
qreal deg_rad(qreal rad);         ///< conversion: degrees <= radians
qreal rad_deg(qreal deg);         ///< conversion: radians <= degrees

}

#endif

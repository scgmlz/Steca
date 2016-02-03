/** \file
 * Core-supporting types and functions.
 */

#include "core_defs.h"

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

namespace core {

///< a range of values (a closed interval)
struct Range {
  Range();
  Range(qreal val);
  Range(qreal min, qreal max);

  qreal min, max;

  void invalidate();              ///< make NaN
  bool isInvalid() const;

  void set(qreal val);            ///< both min and max = val
  void set(qreal min,qreal max);  ///< must be: min<=max
  void safeSet(qreal,qreal);      ///< will be set in the right order min/max

  void extend(qreal val);         ///< extend to include val
  void extend(Range const&);      ///< extend to include the other range
  bool contains(qreal val) const;
};

// TODO see if all three-times-two are needed, disassemble?
struct Borders {
  Range
    gamma,
    tth_regular,
    tth_gamma0; // at gamma=0
};

// cut
struct ImageCut {
  ImageCut(uint top = 0, uint bottom = 0, uint left = 0, uint right = 0);
  bool operator==(ImageCut const&);
  uint top, bottom, left, right;

  uint getWidth(uint imageSize) const;
  uint getHeight(uint imageSize) const;
  uint getCount(uint imageSize) const;
};

/// Image transform - rotation and mirroring
struct ImageTransform {
  enum e {
    NONE            = 0,
    ROTATE_0        = 0,  // no rotation, same as NONE
    ROTATE_1        = 1,  // one quarter
    ROTATE_2        = 2,  // two quarters
    ROTATE_3        = 3,  // three quarters
    MIRROR          = 4,
    MIRROR_ROTATE_0 = MIRROR | ROTATE_0,
    MIRROR_ROTATE_1 = MIRROR | ROTATE_1,
    MIRROR_ROTATE_2 = MIRROR | ROTATE_2,
    MIRROR_ROTATE_3 = MIRROR | ROTATE_3,
  } val;

  ImageTransform(int val);                   ///< clamps val appropriately
  ImageTransform mirror(bool on)     const;  ///< adds/removes the mirror flag
  ImageTransform rotateTo(ImageTransform) const;  ///< rotates, but keeps the mirror flag
  ImageTransform nextRotate()        const;  ///< rotates by one quarter-turn
};

// conversion
qreal deg_rad(qreal rad);         ///< conversion: degrees <= radians
qreal rad_deg(qreal deg);         ///< conversion: radians <= degrees

}

#endif

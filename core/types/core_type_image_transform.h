// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_image_transform.h
//! @brief     The transform type
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPE_IMAGE_TRANSFORM_H
#define CORE_TYPE_IMAGE_TRANSFORM_H

#include "core_defs.h"
#include "core_types_fwd.h"

namespace core {
//------------------------------------------------------------------------------
/// Image transform type - rotation and mirroring (bit-map)

struct ImageTransform {
  enum e {
    ROTATE_0        = 0,  // no transform
    ROTATE_1        = 1,  // one quarter
    ROTATE_2        = 2,  // two quarters
    ROTATE_3        = 3,  // three quarters
    MIRROR          = 4,
    MIRROR_ROTATE_0 = MIRROR | ROTATE_0,
    MIRROR_ROTATE_1 = MIRROR | ROTATE_1,
    MIRROR_ROTATE_2 = MIRROR | ROTATE_2,
    MIRROR_ROTATE_3 = MIRROR | ROTATE_3,
  } val;

  ImageTransform(int val = ROTATE_0);             ///< clamps val appropriately
  ImageTransform mirror(bool on)          const;  ///< adds/removes the mirror flag
  ImageTransform rotateTo(ImageTransform) const;  ///< rotates, but keeps the mirror flag
  ImageTransform nextRotate()             const;  ///< rotates by one quarter-turn

  bool isTransposed() const { return 0 != (val&1); }

  bool operator ==(ImageTransform const& that) const { return val == that.val; }
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_IMAGE_TRANSFORM_H

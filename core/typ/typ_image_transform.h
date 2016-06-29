// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_image_transform.h
//! @brief     The transform type
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_IMAGE_TRANSFORM_H
#define TYP_IMAGE_TRANSFORM_H

#include "def/defs.h"

namespace typ {
//------------------------------------------------------------------------------
// Image transform type - rotation and mirroring (bit-map)

struct ImageTransform {
  CLS(ImageTransform)

  enum eTransform {
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

  // clamps val appropriately
  ImageTransform(uint val = ROTATE_0);

  // adds/removes the mirror flag
  ImageTransform mirror(bool on) const;

  // rotates only; keeps the mirror flag
  ImageTransform rotateTo(rc) const;

  // rotates by one quarter-turn
  ImageTransform nextRotate() const;

  bool isTransposed() const {
    return 0 != (val & 1);
  }

  bool operator==(rc that) const {
    return val == that.val;
  }
};

//------------------------------------------------------------------------------
}
#endif // TYP_IMAGE_TRANSFORM_H

// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      core_type_image_transform.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_type_image_transform.h"

namespace core {

//------------------------------------------------------------------------------

ImageTransform::ImageTransform(uint val_): val((eTransform)(val_ & 7)) {
}

ImageTransform ImageTransform::mirror(bool on) const {
  return on ? ImageTransform(val |  MIRROR)
            : ImageTransform(val & ~MIRROR);
}

ImageTransform ImageTransform::rotateTo(ImageTransform const& rot) const {
  return ImageTransform((val & MIRROR) | (rot.val & 3));
}

ImageTransform ImageTransform::nextRotate() const {
  return rotateTo(val+1);
}

//------------------------------------------------------------------------------
}
// eof

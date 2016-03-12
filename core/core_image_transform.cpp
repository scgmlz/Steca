#include "core_image_transform.h"

namespace core {
//------------------------------------------------------------------------------

ImageTransform::ImageTransform(int val_): val((e)(val_ & 7)) {
}

ImageTransform ImageTransform::mirror(bool on) const {
  return on ? ImageTransform(val |  MIRROR)
            : ImageTransform(val & ~MIRROR);
}

ImageTransform ImageTransform::rotateTo(ImageTransform rot) const {
  return ImageTransform((val & MIRROR) | (rot.val & 3));
}

ImageTransform ImageTransform::nextRotate() const {
  return rotateTo(val+1);
}

//------------------------------------------------------------------------------
}

// eof

#ifndef CORE_IMAGE_TRANSFORM_H
#define CORE_IMAGE_TRANSFORM_H

namespace core {
//------------------------------------------------------------------------------

/// Image transform - rotation and mirroring
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
#endif
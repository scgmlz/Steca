/** \file
 * Detector images.
 */

#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "core_lib.h"
#include "core_array2d.h"

namespace core {

using intens_t = float; ///< short for intensity. float should suffice

class Image final: public Array2D<intens_t> {
  SUPER(Image,Array2D<intens_t>)
public:
  /// Image transform - rotation and mirroring
  struct Transform {
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

    Transform(int val);                   ///< clamps val appropriately
    Transform mirror(bool on)     const;  ///< adds/removes the mirror flag
    Transform rotateTo(Transform) const;  ///< rotates, but keeps the mirror flag
    Transform nextRotate()        const;  ///< rotates by one quarter-turn
  };

  /// Image as vector of intensities, filled with 0 or given intensities.
  Image(QSize const& = QSize(), intens_t const* = nullptr);

  void clear();
  void fill(intens_t val, QSize const&);

  /// Calculate the 1D index of a pixel, with transform.
  uint index(Transform,uint x, uint y) const;

  /// Access single intensity.
  intens_t const& intensity(Transform, uint x, uint y) const;
  /// Set single intensity.
  void setIntensity(Transform, uint x, uint y, intens_t);

  /// Access the whole 1D intensity array, getCount() values.
  intens_t const* getIntensities() const { return getData(); }
  /// Sum all getCount() intensities with new ones.
  void addIntensities(intens_t const*);

  /// Calculate the range of intensity values; cache the result
  Range const& getRgeIntens() const;

private:
  mutable Range rgeIntens;
};

}

#endif

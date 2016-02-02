/** \file
 * Detector images.
 */

#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "core_lib.h"
#include <QSize>

namespace core {

class Image {
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
    Transform rotate()            const;  ///< rotates by one quarter
  };

  using intens_t = float; ///< short for intensity. float should suffice

  /// Image as vector of intensities, filled with 0 or given intensities.
  Image(uint size = 0, intens_t const* = nullptr);

  void clear();                             ///< make empty
  void fill(intens_t defValue, uint size);  ///< allocate with default value
  uint getSize()  const { return size; }

  /// Calculate the 1D index of a pixel, no transform.
  static inline uint index(uint size,uint x, uint y) { return x + y * size; }

  /// Calculate the 1D index of a pixel, with transform.
  uint index(Transform,uint x, uint y) const;

  /// Access single intensity.
  intens_t const& intensity(Transform, uint x, uint y) const;
  /// Set single intensity.
  void setIntensity(Transform, uint x, uint y, intens_t);

  /// Access the whole 1D intensity array.
  intens_t const* getIntensities() const { return intensities.data(); }
  /// Sum all intensities
  void addIntensities(intens_t const*);

  /// Calculate the interval of intensities.
  Interval const& getIntIntens() const;

private:
  uint size; // TODO a simplification - square images; make rect
  QVector<intens_t> intensities;
  mutable Interval intIntens;
};

}

#endif

/** \file
 * Detector images.
 */

#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "core_types.h"
#include "core_array2d.h"

namespace core {
//------------------------------------------------------------------------------

using intens_t = float; ///< short for intensity. float should suffice

class Image final: public Array2D<intens_t> {
  SUPER(Image,Array2D<intens_t>)
public:
  /// Image as vector of intensities, filled with 0 or given intensities.
  Image(QSize const& = QSize(0,0), intens_t const* = nullptr);

  void clear();
  void fill(intens_t val, QSize const&);

  /// Access single intensity.
  intens_t const& intensity(uint i) const {
    return super::at(i);
  }

  /// Set single intensity.
  void setIntensity(uint i, intens_t);

  /// Access the whole 1D intensity array, getCount() values.
  intens_t const* getIntensities() const { return getData(); }
  /// Sum all getCount() intensities with new ones.
  void addIntensities(intens_t const*);

  /// Calculate the range of intensity values; cache the result
  Range const& getRgeIntens() const;

private:
  mutable Range rgeIntens;
};

//------------------------------------------------------------------------------
}
#endif

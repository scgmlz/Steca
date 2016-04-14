// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_image.h
//! @brief     Detector image
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPE_IMAGE_H
#define CORE_TYPE_IMAGE_H

#include "core_type_array2d.h"
#include "core_type_range.h"

namespace core {
//------------------------------------------------------------------------------

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
  Range const& intensRange() const;

private:
  mutable Range rgeIntens;
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_IMAGE_H

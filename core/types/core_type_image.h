// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_type_image.h
//! @brief     Detector image
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CORE_TYPE_IMAGE_H
#define CORE_TYPE_IMAGE_H

#include "core_type_array2d.h"
#include "core_type_range.h"

namespace core {
//------------------------------------------------------------------------------

class Image final : public Array2D<inten_t> {
  SUPER(Image, Array2D<inten_t>)
public:
  /// Image as vector of intensities, filled with 0 or given intensities.
  Image(QSize const& = QSize(0, 0), inten_t const* = nullptr);

  Image(rcImage);

  /// Access single intensity.
  inten_t inten(uint i) const { return super::at(i); }

  /// Access single intensity.
  inten_t inten(uint i, uint j) const { return super::at(i, j); }

  /// Set single intensity.
  void setInten(uint i, inten_t inten) { super::setAt(i, inten); }

  /// Access the whole 1D intensity array, getCount() values.
  inten_t const* intensData() const { return data(); }

  /// Sum all intensities with new ones.
  void addIntens(rcImage) THROWS;
  void addIntens(inten_t const*);
};

//------------------------------------------------------------------------------
}
#endif  // CORE_TYPE_IMAGE_H

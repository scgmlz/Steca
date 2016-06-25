// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_image.h
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

#ifndef TYP_IMAGE_H
#define TYP_IMAGE_H

#include "typ/typ_array2d.h"
#include "typ_types.h"

namespace typ {
//------------------------------------------------------------------------------

class Image final : public Array2D<inten_t> {
  CLS(Image) SUPER(Array2D<inten_t>)
public:
  // Image as vector of intensities, filled with 0 or given intensities.
  Image(size2d::rc = size2d(0, 0), inten_t const* = nullptr);

  Image(rc);

  // Access single intensity.
  inten_t inten(uint i) const {
    return super::at(i);
  }

  // Access single intensity.
  inten_t inten(uint i, uint j) const {
    return super::at(i, j);
  }

  // Set single intensity.
  void setInten(uint i, inten_t inten) {
    super::setAt(i, inten);
  }

  // Access the whole 1D intensity array, getCount() values.
  not_null<inten_t const*> intensData() const {
    return not_null<inten_t const*>::from(data());
  }

  // Sum all intensities with new ones.
  void addIntens(Image::rc) THROWS;
  void addIntens(not_null<inten_t const*>);
};

//------------------------------------------------------------------------------
}
#endif // TYP_IMAGE_H

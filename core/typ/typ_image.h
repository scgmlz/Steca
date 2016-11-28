// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the LICENSE and AUTHORS files for more details.
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

  inten_rge::rc rgeInten() const {
    return rgeInten_;
  }

private:
  inten_rge rgeInten_;
};

//------------------------------------------------------------------------------
}
#endif // TYP_IMAGE_H

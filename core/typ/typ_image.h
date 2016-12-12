/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef TYP_IMAGE_H
#define TYP_IMAGE_H

#include "lib/typ/typ_array2d.h"
#include "typ_types.h"

namespace typ {
//------------------------------------------------------------------------------

typedef Array2D<inten_t> inten_arr;

class Image final {
  CLS(Image)
public:
  // Image as vector of intensities, filled with 0 or given intensities.
  Image(size2d::rc = size2d(0, 0));
  Image(inten_arr::rc);

  size2d::rc size() const {
    return intens_.size();
  }

  void clear() {
    return intens_.clear();
  }

  bool isEmpty() const {
    return intens_.isEmpty();
  }

  void fill(inten_t val, size2d::rc size) {
    intens_.fill(val, size);
  }

  inten_t inten(uint i) const {
    return intens_.at(i);
  }

  inten_t inten(uint i, uint j) const {
    return intens_.at(i, j);
  }

  void setInten(uint i, inten_t val) {
    intens_.setAt(i, val);
  }

  void setInten(uint i, uint j, inten_t val) {
    intens_.setAt(i, j, val);
  }

  void addInten(uint i, uint j, inten_t val) {
    intens_.refAt(i, j) += val;
  }

  // Sum all intensities with new ones.
  void addIntens(Cls::rc) THROWS;

  inten_rge::rc rgeInten() const {
    return rgeInten_;
  }

private:
  inten_arr intens_;
  inten_rge rgeInten_;
};

//------------------------------------------------------------------------------
}
#endif // TYP_IMAGE_H

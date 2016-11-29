/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
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

#include "typ/typ_array2d.h"
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

  // Sum all intensities with new ones.
  void addIntens(inten_arr::rc) THROWS;

  inten_rge::rc rgeInten() const {
    return rgeInten_;
  }

private:
  inten_arr intens;
  inten_rge rgeInten_;
};

//------------------------------------------------------------------------------
}
#endif // TYP_IMAGE_H

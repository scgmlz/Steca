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

#include "typ_image.h"

namespace typ {
//------------------------------------------------------------------------------

Image::Image(size2d::rc size) {
  fill(0, size);
}

Image::Image(inten_arr::rc that): Image(that.size()) {
  addIntens(that);
}

void Image::addIntens(Cls::rc that) THROWS {
  RUNTIME_CHECK(size() == that.size(), "inconsistent image size");
  auto w = size().w, h = size().h;
  for (uint i=0; i<w; ++i)
    for (uint j=0; j<h; ++j) {
      inten_t inten = that.inten(i, j);
      rgeInten_.extendBy(inten);
      addInten(i, j, inten);
    }
}

//------------------------------------------------------------------------------
}
// eof

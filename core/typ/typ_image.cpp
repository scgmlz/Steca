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

#include "typ_image.h"

namespace typ {
//------------------------------------------------------------------------------

Image::Image(size2d::rc size, inten_t const* src) {
  fill(0, size);
  if (src)
    addIntens(not_null<inten_t const*>::from(src));
}

Image::Image(Image::rc that)
: Image(that.size(), that.intensData()) {
}

void Image::addIntens(Image::rc that) THROWS {
  RUNTIME_CHECK(size() == that.size(), "inconsistent image size");
  addIntens(not_null<inten_t const*>::from(that.data()));
}

void Image::addIntens(not_null<inten_t const*> thatIntens) {
  inten_t *dst = data();
  inten_t const *src = thatIntens;
  for_i (count()) {
    inten_t inten = *src++;
    rgeInten_.extendBy(inten);
    *dst++ += inten;
  }
}

//------------------------------------------------------------------------------
}
// eof

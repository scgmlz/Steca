// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_image.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
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

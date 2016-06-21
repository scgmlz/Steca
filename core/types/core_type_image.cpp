// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_type_image.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "core_type_image.h"

namespace core {
//------------------------------------------------------------------------------

Image::Image(size2d const& size, inten_t const* src) {
  fill(0, size);
  if (src) addIntens(src);
}

Image::Image(rcImage that) : Image(that.size(), that.intensData()) {}

void Image::addIntens(rcImage that) THROWS {
  RUNTIME_CHECK(size() == that.size(), "inconsistent image size");
  addIntens(that.data());
}

void Image::addIntens(inten_t const* thatIntens) {
  auto intens = data();
  uint n      = count();
  while (n-- > 0) {
    *intens++ += *thatIntens++;
  }
}

//------------------------------------------------------------------------------
}
// eof

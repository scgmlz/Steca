// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_image.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_type_image.h"

namespace core {

//------------------------------------------------------------------------------

Image::Image(QSize const& size, inten_t const* src) {
  fill(0,size);
  if (src) addIntens(src);
}

void Image::addIntens(rcImage that) THROWS {
  RUNTIME_CHECK(size() == that.size(),"inconsistent image size");
  addIntens(that.data());
}

void Image::addIntens(inten_t const* thatIntens) {
  auto intens = data();
  uint n = count();
  while(n-- > 0) { 
    *intens++ += *thatIntens++; 
  }
}

//------------------------------------------------------------------------------
}
// eof

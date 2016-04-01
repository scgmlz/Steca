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

Image::Image(QSize const& size, intens_t const* src) {
  fill(0,size);
  addIntensities(src);
}

void Image::clear() {
  super::clear();
  rgeIntens.invalidate();
}

void Image::fill(intens_t val, QSize const& size) {
  super::fill(val,size);
  rgeIntens.set(val);
}

void Image::setIntensity(uint i, intens_t val) {
  super::setAt(i,val);
  rgeIntens.invalidate();
}

void Image::addIntensities(intens_t const* src) {
  if (src) {
    auto data  = getData();
    uint count = getCount();
    while(count-- > 0) *data++ += *src++;
    rgeIntens.invalidate();
  }
}

Range const& Image::intensRange() const {
  if (!rgeIntens.isValid()) {
    auto data    = getData();
    uint count   = getCount();
    while(count-- > 0) rgeIntens.extendBy(*data++);
  }

  return rgeIntens;
}

//------------------------------------------------------------------------------
}
// eof

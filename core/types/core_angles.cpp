// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_angles.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_angles.h"
#include <qmath.h>

namespace core {

deg::deg(rad const& r) {
  val_ = r.toDeg();
}

rad deg::toRad() const {
  return val_ * (M_PI / 180);
}

//------------------------------------------------------------------------------

rad::rad(deg const& d) {
  val_ = d.toRad();
}

deg rad::toDeg() const {
  return val_ * (180 / M_PI);
}

//------------------------------------------------------------------------------
}
// eof

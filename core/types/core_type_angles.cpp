// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_angles.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_type_angles.h"

namespace core {
//------------------------------------------------------------------------------

DiffractionAngles::DiffractionAngles(qreal gamma_, qreal tth_)
  : gamma(gamma_), tth(tth_) {
}

//------------------------------------------------------------------------------

DiffractionAnglesMap::DiffractionAnglesMap() {
}

//------------------------------------------------------------------------------
}
// eof

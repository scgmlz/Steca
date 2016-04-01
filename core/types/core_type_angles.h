// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_angles.h
//! @brief     Diffraction angles pair etc.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPE_ANGLES_H
#define CORE_TYPE_ANGLES_H

#include "core_defs.h"
#include "core_types_fwd.h"

namespace core {
//------------------------------------------------------------------------------
/// Diffraction angles pair

struct DiffractionAngles {
  DiffractionAngles(qreal gamma = 0, qreal tth = 0);

  qreal gamma, tth;
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_ANGLES_H

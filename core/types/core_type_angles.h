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
#include "types/core_type_array2d.h"

namespace core {
//------------------------------------------------------------------------------
/// Diffraction angles pair

struct DiffractionAngles {
  DiffractionAngles(qreal gamma = 0, qreal tth = 0);

  qreal gamma, tth;
};

//------------------------------------------------------------------------------
// TODO make smart caching

class DiffractionAnglesMap: public Array2D<DiffractionAngles> {
  SUPER(DiffractionAnglesMap,Array2D<DiffractionAngles>)
public:
  DiffractionAnglesMap();
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_ANGLES_H

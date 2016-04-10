// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      refhub.h
//! @brief     Reference to The Hub.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef REFHUB_H
#define REFHUB_H

#include "core_defs.h"

namespace gui {
class TheHub;
}

//------------------------------------------------------------------------------
/// The base class that refers to the hub + support methods.

class RefHub {
public:
  RefHub(gui::TheHub&);
protected:
  gui::TheHub &_theHub;
};

//------------------------------------------------------------------------------
#endif // REFHUB_H

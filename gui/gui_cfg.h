// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      gui_cfg.h
//! @brief     GUI configuration.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef GUI_CFG_H
#define GUI_CFG_H

#include "def/defs.h"

namespace gui_cfg {
//------------------------------------------------------------------------------

#ifdef Q_OS_WIN

uint const em4_2 = 8; // widget width for 4.2 numbers
uint const em4   = 5; // for 4 numbers

#else

uint const em4_2 = 6;
uint const em4   = 4;

#endif

//------------------------------------------------------------------------------
}
#endif

// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/gui_cfg.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef GUI_CFG_H
#define GUI_CFG_H

#include "def/defs.h"

namespace gui_cfg {
//------------------------------------------------------------------------------

#ifdef Q_OS_WIN

uint const em4_2 = 8; // widget width for 4.2 numbers
uint const em4 = 5; // for 4 numbers

#else

uint const em4_2 = 6;
uint const em4 = 4;

#endif

//------------------------------------------------------------------------------
}
#endif

/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

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

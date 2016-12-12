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

#ifndef TYP_STR_H
#define TYP_STR_H

#include "def/def_macros.h"
#include <QString>
#include <QStringBuilder>

//------------------------------------------------------------------------------

// string support
typedef QString     str;        // a short alias for the QString class
typedef str const&  rcstr;      // a reference to a string constant
typedef char const* pcstr;      // C-style (zero-byte-terminated) string

extern  str const   EMPTY_STR;  // an empty string

//------------------------------------------------------------------------------
#endif // TYP_STR_H

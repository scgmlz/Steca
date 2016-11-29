/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
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

#include "typ_log.h"

//------------------------------------------------------------------------------

void MessageLogger::info(rcstr msg) {
  if (handler)
    handler(msg, INFO);
}

void MessageLogger::warn(rcstr msg) {
  if (handler)
    handler(msg, WARN);
}

void MessageLogger::popup(rcstr msg) {
  if (handler)
    handler(msg, POPUP);
}

void (*MessageLogger::handler)(rcstr, eType) = nullptr;

//------------------------------------------------------------------------------
// eof

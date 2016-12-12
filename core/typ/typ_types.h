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

#ifndef TYP_TYPES_H
#define TYP_TYPES_H

#include "lib/typ/typ_angles.h"
#include "lib/typ/typ_range.h"
#include "lib/typ/typ_strlst.h"
#include "lib/typ/typ_vec.h"
#include "lib/typ/typ_xy.h"

typedef typ::deg gma_t;
typedef typ::deg tth_t;

typedef float inten_t;
typedef float fwhm_t;

typedef typ::vec<inten_t> inten_vec;

typedef typ::Range inten_rge;
typedef typ::Range tth_rge;
typedef typ::Range gma_rge;

typedef typ::XY peak_t;

enum class eNorm {
  NONE,
  MONITOR, DELTA_MONITOR, DELTA_TIME, BACKGROUND,
};

str_lst::rc normStrLst();

//------------------------------------------------------------------------------
#endif // TYP_TYPES_H

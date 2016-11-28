// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the COPYING and AUTHORS files for more details.
// ************************************************************************** //

#ifndef CONFIG_H
#define CONFIG_H

#include "def/defs.h"

namespace config_key {
//------------------------------------------------------------------------------

extern str const

// configuration
  GROUP_CONFIG,
  STARTUP_ABOUT, STARTUP_CHECK_UPDATE,
  DETECTOR, DET_DISTANCE, DET_PIX_SIZE,

// main window
  GROUP_MAINWIN,
  GEOMETRY,
  STATE,

// session
  FILES, SELECTED_FILES, CORR_FILE,
  COMBINE,
  CUT, LEFT, TOP, RIGHT, BOTTOM,
  BEAM_OFFSET,
  TRANSFORM,
  BG_DEGREE, BG_RANGES,
  REFLECTIONS,

// output
  GROUP_OUTPUT,
  NUM_SLICES,
  LIMIT_GAMMA,

  INTERPOLATED,

  STEP_ALPHA,
  STEP_BETA,
  IDW_RADIUS,

  AVG_ALPHA_MAX,
  AVG_RADIUS, AVG_THRESHOLD,

  SAVE_DIR, SAVE_FMT;
}

//------------------------------------------------------------------------------
#endif

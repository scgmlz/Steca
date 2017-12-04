// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/config.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //


#ifndef CONFIG_H
#define CONFIG_H

#include "def/defs.h"

namespace config_key {
//------------------------------------------------------------------------------

extern str const

// configuration
  GROUP_CONFIG,
  CURRENT_VERSION, STARTUP_ABOUT, STARTUP_CHECK_UPDATE,
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
  BG_DEGREE, BG_RANGES, INTEN_SCALED_AVG, INTEN_SCALE,
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

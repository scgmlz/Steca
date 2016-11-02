// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      thehub.h
//! @brief     The communication hub.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
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

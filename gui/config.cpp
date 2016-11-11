// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      thehub.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "config.h"

namespace config_key {
//------------------------------------------------------------------------------

str const
  GROUP_CONFIG("config"),
  STARTUP_ABOUT("startup about"), STARTUP_CHECK_UPDATE("startup check update"),
  DETECTOR("detector"), DET_DISTANCE("distance"), DET_PIX_SIZE("pixel size"),

  GROUP_MAINWIN("MainWin"),
  GEOMETRY("geometry"), STATE("state"),

  FILES("files"), SELECTED_FILES("selected files"), CORR_FILE("correction file"),
  COMBINE("combine"),
  CUT("cut"), LEFT("left"), TOP("top"), RIGHT("right"), BOTTOM("bottom"),
  BEAM_OFFSET("beam offset"),
  TRANSFORM("image transform"),
  BG_DEGREE("background degree"), BG_RANGES("background ranges"),
  REFLECTIONS("reflections"),

// output
  GROUP_OUTPUT("output"),
  NUM_SLICES("num slices"),
  LIMIT_GAMMA("limit gamma"),

  INTERPOLATED("interpolated"),

  STEP_ALPHA("step alpha"),
  STEP_BETA("step beta"),
  IDW_RADIUS("idw radius"),

  AVG_ALPHA_MAX("avg alpha max"),
  AVG_RADIUS("avg radius"),
  AVG_THRESHOLD("avg threshold"),

  SAVE_DIR("save dir"),
  SAVE_FMT("save format");

//------------------------------------------------------------------------------
}
// eof

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

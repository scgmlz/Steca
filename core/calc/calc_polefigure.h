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

#ifndef CALC_POLEFIGURE_H
#define CALC_POLEFIGURE_H

#include "calc_reflection_info.h"
#include "typ/typ_types.h"
#include "typ/typ_async.h"

namespace calc {
//------------------------------------------------------------------------------

struct itf_t {
  CLS(itf_t)

  itf_t();
  itf_t(inten_t, tth_t, fwhm_t);

  void operator+=(rc);

  inten_t inten;
  tth_t   tth;
  fwhm_t  fwhm;
};

typedef typ::vec<itf_t> itfs_t;

// Interpolates reflection infos to a single point using idw.
itf_t interpolateValues(typ::deg searchRadius, ReflectionInfos::rc infos,
                        typ::deg alpha, typ::deg beta);

ReflectionInfos interpolate(ReflectionInfos::rc,
                            typ::deg alphaStep, typ::deg betaStep, typ::deg idwRadius,
                            typ::deg averagingAlphaMax, typ::deg averagingRadius, qreal inclusionTreshold,
                            Progress*);

//------------------------------------------------------------------------------
}
#endif // CALC_POLEFIGURE_H

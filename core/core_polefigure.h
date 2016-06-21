// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_polefigure.h
//! @brief     Pole figure
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CORE_POLEFIGURE_H
#define CORE_POLEFIGURE_H

#include "core_reflection_info.h"
#include "types/core_angles.h"

class Progress;

namespace core { namespace pole {
//------------------------------------------------------------------------------

struct itf_t {
  itf_t();
  itf_t(inten_t inten, deg tth, qreal fwhm);

  void operator+=(itf_t const&);

  inten_t inten;
  deg     tth;
  qreal   fwhm;
};

typedef vec<itf_t> itfs_t;

// Interpolates reflection infos to a single point using idw.
itf_t interpolateValues(deg searchRadius, ReflectionInfos const& infos,
                        deg alpha, deg beta);

ReflectionInfos interpolate(ReflectionInfos const&, deg alphaStep, deg betaStep,
                            deg averagingAlphaMax, deg averagingRadius,
                            deg idwRadius, qreal inclusionTreshold);

//------------------------------------------------------------------------------
}}
#endif  // CORE_POLEFIGURE_H

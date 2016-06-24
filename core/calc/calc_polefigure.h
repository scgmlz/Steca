// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      calc_polefigure.h
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

#ifndef CALC_POLEFIGURE_H
#define CALC_POLEFIGURE_H

#include "calc_reflection_info.h"
#include "typ/typ_types.h"
//#include "typ/typ_angles.h"
//#include "typ/typ_vec.h"

class Progress;

namespace calc {
//------------------------------------------------------------------------------

struct itf_t {
  THIS(itf_t)

  itf_t();
  itf_t(typ::inten_t, typ::deg tth, qreal fwhm);

  void operator+=(rc);

  typ::inten_t inten;
  typ::deg     tth;
  qreal        fwhm;
};

typedef typ::vec<itf_t> itfs_t;

// Interpolates reflection infos to a single point using idw.
itf_t interpolateValues(typ::deg searchRadius, ReflectionInfos::rc infos,
                        typ::deg alpha, typ::deg beta);

ReflectionInfos interpolate(ReflectionInfos::rc, typ::deg alphaStep, typ::deg betaStep,
                            typ::deg averagingAlphaMax, typ::deg averagingRadius,
                            typ::deg idwRadius, qreal inclusionTreshold);

//------------------------------------------------------------------------------
}
#endif // CALC_POLEFIGURE_H

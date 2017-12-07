// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_polefigure.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CALC_POLEFIGURE_H
#define CALC_POLEFIGURE_H

#include "calc_reflection_info.h"
#include "typ/typ_async.h"

namespace calc {

struct itf_t {
    CLASS(itf_t)

    itf_t();
    itf_t(inten_t, deg, fwhm_t);

    void operator+=(rc);

    inten_t inten;
    deg tth;
    fwhm_t fwhm;
};

typedef typ::vec<itf_t> itfs_t;

// Interpolates reflection infos to a single point using idw.
itf_t interpolateValues(
    typ::deg searchRadius, ReflectionInfos::rc infos, typ::deg alpha, typ::deg beta);

ReflectionInfos interpolate(
    ReflectionInfos::rc, typ::deg alphaStep, typ::deg betaStep, typ::deg idwRadius,
    typ::deg averagingAlphaMax, typ::deg averagingRadius, qreal inclusionTreshold, Progress*);
}
#endif // CALC_POLEFIGURE_H

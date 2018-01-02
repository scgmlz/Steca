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
#include "typ/async.h"

namespace calc {

struct itf_t {
    itf_t();
    itf_t(inten_t, deg, fwhm_t);

    void operator+=(itf_t const&);

    inten_t inten;
    deg tth;
    fwhm_t fwhm;
};

typedef vec<itf_t> itfs_t;

// Interpolates reflection infos to a single point using idw.
itf_t interpolateValues(
    deg searchRadius, ReflectionInfos const& infos, deg alpha, deg beta);

ReflectionInfos interpolate(
    ReflectionInfos const&, deg alphaStep, deg betaStep, deg idwRadius,
    deg averagingAlphaMax, deg averagingRadius, qreal inclusionTreshold, Progress*);
}
#endif // CALC_POLEFIGURE_H

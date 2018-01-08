// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/calc_polefigure.h
//! @brief     Defines function interpolateInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CALC_POLEFIGURE_H
#define CALC_POLEFIGURE_H

#include "core/calc/calc_reflection_info.h"

class Progress;

ReflectionInfos interpolateInfos(
    ReflectionInfos const&, deg alphaStep, deg betaStep, deg idwRadius,
    deg averagingAlphaMax, deg averagingRadius, qreal inclusionTreshold, Progress*);

#endif // CALC_POLEFIGURE_H

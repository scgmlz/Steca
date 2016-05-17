// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      core_polefigure.h
//! @brief     Pole figure
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_POLEFIGURE_H
#define CORE_POLEFIGURE_H

#include "core_reflection_info.h"
#include "types/core_angles.h"

namespace core { namespace pole {
//------------------------------------------------------------------------------

ReflectionInfos interpolate(ReflectionInfos const&,
    deg alphaStep, deg betaStep,
    deg averagingAlphaMax, deg averagingRadius,
    deg idwRadius,
    qreal inclusionTreshold);

//------------------------------------------------------------------------------
}}
#endif // CORE_POLEFIGURE_H

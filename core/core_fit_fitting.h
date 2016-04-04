// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_fit_fitting.h
//! @brief     Fitting.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_FIT_FITTING_H
#define CORE_FIT_FITTING_H

#include "core_fit_functions.h"

namespace core {

class Curve;

namespace fit {
//------------------------------------------------------------------------------

/// Background fit
Polynomial fitBackground(core::Curve const&, core::Ranges const&, uint degree);

/// Peak fit
void fitPeak(PeakFunction&, core::Curve const&, core::Range const&);

//------------------------------------------------------------------------------
}}
#endif

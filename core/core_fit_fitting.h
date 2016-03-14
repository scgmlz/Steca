/** \file
 * Fitting
 */

#ifndef CORE_FIT_FITTING_H
#define CORE_FIT_FITTING_H

#include "core_fit_functions.h"

namespace core {

class Curve;

namespace fit {
//------------------------------------------------------------------------------

/// Polynomial fit
Polynomial fitBackground(core::Curve const& dgram, core::Ranges const&, uint degree);

/// Peak fit
void fitPeak(PeakFunction&, core::Curve const&, core::Range const&);

//------------------------------------------------------------------------------
}}
#endif

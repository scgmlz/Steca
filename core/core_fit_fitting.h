/** \file
 * Fitting
 */

#ifndef CORE_FIT_FITTING_H
#define CORE_FIT_FITTING_H

#include "core_fit_functions.h"

namespace core { namespace fit {
//------------------------------------------------------------------------------

/// Polynomial fit
Polynomial fitBackground(Curve const& dgram, core::Ranges const&, uint degree);

/// Peak fit
void fitPeak(PeakFunction&, Curve const&, core::Range const&);

//------------------------------------------------------------------------------
}}
#endif

/** \file
 * Fitting
 */

#ifndef CORE_FIT_FITTING_H
#define CORE_FIT_FITTING_H

#include "core_fit_functions.h"

namespace core { namespace fit {
//------------------------------------------------------------------------------

/// Polynomial fit
Polynomial fitBackground(TI_Curve const& dgram, core::Ranges const&, uint degree);

//------------------------------------------------------------------------------
}}
#endif

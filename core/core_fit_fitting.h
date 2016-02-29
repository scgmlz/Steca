/** \file
 * Fitting
 */

#ifndef CORE_FIT_FITTING_H
#define CORE_FIT_FITTING_H

#include "core_types.h"

namespace core { namespace fit {
//------------------------------------------------------------------------------

/// Polynomial fit
TI_Curve fitBackground(TI_Curve const& dgram, core::Ranges const&, uint degree);

//------------------------------------------------------------------------------
}}
#endif

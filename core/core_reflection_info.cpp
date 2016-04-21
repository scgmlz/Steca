// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_reflection_info.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_reflection_info.h"
//#include "core_session.h" // TODO OUT
//#include "core_fit_fitting.h"
//#include "types/core_type_matrix.h"
//#include "types/core_type_curve.h"
//#include <qmath.h>

namespace core{
//------------------------------------------------------------------------------

/* NOTE Invalid parameters are set to NaNs. However, some analysis programs
 * require -1 as unknown value; thus, NaN parameter values should be output
 * as -1 when output is written for these programs (polefigure!).
 */

ReflectionInfo::ReflectionInfo()
  : alpha_(qQNaN()), beta_(qQNaN()), rgeGamma_()
  , inten_(qQNaN()), tth_(qQNaN()), fwhm_(qQNaN())
{
}

ReflectionInfo::ReflectionInfo(qreal alpha, qreal beta, rcRange rgeGamma,
                               qreal inten, qreal tth, qreal fwhm)
: alpha_(alpha), beta_(beta), rgeGamma_(rgeGamma)
, inten_(inten), tth_(tth), fwhm_(fwhm)
{
}

//------------------------------------------------------------------------------
}

// eof

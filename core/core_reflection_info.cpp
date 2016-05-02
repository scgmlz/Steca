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

namespace core {
//------------------------------------------------------------------------------

/* NOTE Invalid parameters are set to NaNs. However, some analysis programs
 * require -1 as unknown value; thus, NaN parameter values should be output
 * as -1 when output is written for these programs (polefigure!).
 */

ReflectionInfo::ReflectionInfo()
: ReflectionInfo(qQNaN(),qQNaN(),Range(),qQNaN(),qQNaN(),qQNaN()) {
}

ReflectionInfo::ReflectionInfo(deg alpha, deg beta, rcRange rgeGamma,
                               qreal inten, deg tth, qreal fwhm)
: alpha_(alpha), beta_(beta), rgeGamma_(rgeGamma)
, inten_(inten), tth_(tth), fwhm_(fwhm)
{
}

ReflectionInfo::ReflectionInfo(deg alpha, deg beta, rcRange rgeGamma)
: ReflectionInfo(alpha,beta,rgeGamma,qQNaN(),qQNaN(),qQNaN()) {
}


//------------------------------------------------------------------------------

void ReflectionInfos::invalidate() {
  avgInten = qQNaN();
  rgInten.invalidate();
}

qreal& ReflectionInfos::averageInten() const {
  for_i (count()) {
    avgInten += at(i).inten();
  }
  avgInten /= count();
  return avgInten;
}

rcRange ReflectionInfos::rgeInten() const {
  for_i (count()) {
    rgInten.extendBy(at(i).inten());
  }
  return rgInten;
}

void ReflectionInfos::append(rcReflectionInfo info) {
  super::append(info);
  invalidate();
}


//------------------------------------------------------------------------------
}

// eof

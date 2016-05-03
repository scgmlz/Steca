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

/* NOTE Invalid output parameters are set to NaNs. However, some analysis programs
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

void ReflectionInfos::append(rcReflectionInfo info) {
  super::append(info);
  invalidate();
}

qreal ReflectionInfos::averageInten() const {
  if (qIsNaN(avgInten_)) {
    avgInten_ = 0;
    uint cnt = count();
    if (cnt) {
      for_i (cnt)
        avgInten_ += at(i).inten();
      avgInten_ /= cnt;
    }
  }

  return avgInten_;
}

rcRange ReflectionInfos::rgeInten() const {
  if (!rgeInten_.isValid()) {
    for_i (count())
      rgeInten_.extendBy(at(i).inten());
  }

  return rgeInten_;
}

void ReflectionInfos::invalidate() {
  avgInten_ = qQNaN();
  rgeInten_.invalidate();
}

//------------------------------------------------------------------------------
}

// eof

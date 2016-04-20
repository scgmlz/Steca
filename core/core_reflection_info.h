// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_reflection_info.h
//! @brief     Reflection data.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef REFLECTION_INFO_H
#define REFLECTION_INFO_H

#include "types/core_coords.h"
#include "types/core_type_range.h"

namespace core {
//------------------------------------------------------------------------------

class ReflectionInfo final {
public:
  ReflectionInfo();
  ReflectionInfo(qreal alpha, qreal beta, rcRange rgeGamma,
                 qreal inten, qreal tth, qreal fwhm);

//OUT  qreal   alpha()         const { return alpha_;        }
//  qreal   beta()          const { return beta_;         }
//  rcRange gammaRange()    const { return rgeGamma_;   }
//  rcXY    peakPosition()  const { return peakPosition_; }
//  qreal   peakFWHM()      const { return peakFWHM_;     }

//OUT  static ReflectionInfo make(rcSession,
//                             rcDatasets,
//                             rcDataset,
//                             rcReflection,
//                             rcRange rgeTth,
//                             rcRange gammaSector);

private:
    qreal alpha_, beta_; Range rgeGamma_;
    qreal inten_, tth_, fwhm_;
};

//------------------------------------------------------------------------------
}
#endif

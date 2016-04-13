// TODO HEADER
#ifndef REFLECTION_INFO_H
#define REFLECTION_INFO_H

#include "types/core_coords.h"
#include "types/core_type_range.h"

namespace core {
//------------------------------------------------------------------------------

class Session; // REMOVE

class ReflectionInfo final {
public:
  ReflectionInfo();
  ReflectionInfo(qreal alpha, qreal beta,
                 rcRange rgeGamma,
                 rcXY  peakPosition,
                 qreal peakFWHM);

  qreal   alpha()         const { return alpha_;        }
  qreal   beta()          const { return beta_;         }
  rcRange gammaRange()    const { return gammaRange_;   }
  rcXY    peakPosition()  const { return peakPosition_; }
  qreal   peakFWHM()      const { return peakFWHM_;     }

  static ReflectionInfo make(rcSession,
                             rcDatasets,
                             rcDataset,
                             Reflection const&,
                             rcRange rgeTth,
                             rcRange gammaSector);

private:
    qreal alpha_;
    qreal beta_;
    Range gammaRange_;
    XY    peakPosition_;
    qreal peakFWHM_;
};

//------------------------------------------------------------------------------
}
#endif

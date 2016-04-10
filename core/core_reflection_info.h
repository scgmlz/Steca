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

  qreal   alpha()         const { return _alpha;        }
  qreal   beta()          const { return _beta;         }
  rcRange gammaRange()    const { return _gammaRange;   }
  rcXY    peakPosition()  const { return _peakPosition; }
  qreal   peakFWHM()      const { return _peakFWHM;     }

  static ReflectionInfo make(rcSession,
                             rcDatasets,
                             rcDataset,
                             Reflection const&,
                             rcRange rgeTth,
                             rcRange gammaSector);

private:
    qreal _alpha;
    qreal _beta;
    Range _gammaRange;
    XY    _peakPosition;
    qreal _peakFWHM;
};

//------------------------------------------------------------------------------
}
#endif

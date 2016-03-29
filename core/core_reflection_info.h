#ifndef REFLECTION_INFO_H
#define REFLECTION_INFO_H

#include "core_types.h"

namespace core {
//------------------------------------------------------------------------------

class ReflectionInfo final {
public:
  ReflectionInfo();
  ReflectionInfo(qreal const alpha_,
                 qreal const beta_,
                 Range const& gammaRange_,
                 XY const& peakPosition_,
                 qreal const peakFWHM_);

  qreal getAlpha() const { return alpha; }
  qreal getBeta() const { return beta; }
  Range const& getGammaRange() const { return gammaRange; }
  XY const& getPeakPosition() const { return peakPosition; }
  qreal getPeakFWHM() const { return peakFWHM; }

private:
    qreal alpha;
    qreal beta;
    Range gammaRange;
    XY peakPosition;
    qreal peakFWHM;
};

//------------------------------------------------------------------------------
}
#endif

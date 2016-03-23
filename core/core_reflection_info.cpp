#include "core_reflection_info.h"

namespace core{
//------------------------------------------------------------------------------

ReflectionInfo::ReflectionInfo(qreal const alpha_,
                               qreal const beta_,
                               Range const& gammaRange_,
                               XY const& peakPosition_,
                               qreal const peakFWHM_)
:  alpha(alpha_)
  ,beta(beta_)
  ,gammaRange(gammaRange_)
  ,peakPosition(peakPosition_)
  ,peakFWHM(peakFWHM_)
{
}

//------------------------------------------------------------------------------
}

// eof

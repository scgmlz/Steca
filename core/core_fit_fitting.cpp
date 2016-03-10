#include "core_fit_fitting.h"
#include "core_fit_methods.h"
#include "core_fit_limits.h"

namespace core { namespace fit {
//------------------------------------------------------------------------------

Polynomial fitBackground(TI_Curve const& dgram, core::Ranges const& bgRanges, uint degree) {
  Polynomial bgPolynomial(qMin(degree,MAX_BACKGROUND_POLYNOMIAL_DEGREE));
  FittingLevenbergMarquardt().fitWithoutCheck(bgPolynomial,dgram.intersect(bgRanges));
  return bgPolynomial;
}

void fitPeak(PeakFunction& peakFunction, TI_Curve const& dgram, core::Range const& range) {
  peakFunction.reset();

  core::Curve curve = dgram.intersect(range);
  if (curve.isEmpty()) return;

  if (!peakFunction.getGuessPeak().isDefined()) { // calculate guesses
    uint peakIndex  = curve.maxYindex();
    auto peakTth    = curve.x(peakIndex);
    auto peakIntens = curve.y(peakIndex);

    // half-maximum indices
    uint hmi1 = peakIndex, hmi2 = peakIndex;

    // left
    for (uint i=peakIndex; i-- > 0; ) {
      hmi1 = i;
      if (curve.y(i) < peakIntens/2) break;
    }

    // right
    for (uint i=peakIndex, iCnt=curve.count(); i < iCnt; ++i) {
      hmi2 = i;
      if (curve.y(i) < peakIntens/2) break;
    }

    peakFunction.setGuessPeak(XY(peakTth,peakIntens));
    peakFunction.setGuessFWHM(curve.x(hmi2) - curve.x(hmi1));
  }

  FittingLevenbergMarquardt().fitWithoutCheck(peakFunction,curve);
}

//------------------------------------------------------------------------------
}}
// eof

#include "core_fit_fitting.h"
#include "core_fit_methods.h"
#include "core_fit_limits.h"

namespace core { namespace fit {
//------------------------------------------------------------------------------

Polynomial fitBackground(TI_Curve const& dgram, core::Ranges const& bgRanges, uint degree) {
  core::Curve curve;

  // The following adds points that are in ranges to the curve
  // it works because both ranges and dgram are ordered and ranges are non-overlapping
  ASSERT(dgram.isOrdered())

  auto tth   = dgram.getTth();
  auto inten = dgram.getInten();

  uint di = 0, count = dgram.count();
  for_i (bgRanges.count()) {
    auto const& range = bgRanges.at(i);
    while (di<count && tth[di] <  range.min)
      ++di;
    while (di<count && tth[di] <= range.max) {
      curve.append(tth[di],inten[di]);
      ++di;
    }
  }

  Polynomial bgPolynomial(qMin(degree,MAX_BACKGROUND_POLYNOMIAL_DEGREE));
  FittingLevenbergMarquardt().fitWithoutCheck(bgPolynomial,curve);

  return bgPolynomial;
}

//------------------------------------------------------------------------------
}}
// eof

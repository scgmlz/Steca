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

Gaussian fitPeak(TI_Curve const& dgram, core::Range const& range) {
  core::Curve curve;
  ASSERT(dgram.isOrdered())

  auto tth   = dgram.getTth();
  auto inten = dgram.getInten();

  qreal peakTth = 0, peakIntens = 0, fwhm = qQNaN();
  uint di = 0, count = dgram.count();
  while (di<count && tth[di] <  range.min)
    ++di;
  while (di<count && tth[di] <= range.max) {
    qreal t = tth[di], in = inten[di];
    curve.append(t,in);
    ++di;

    if (peakIntens < in) {
      peakTth = t; peakIntens = in;
    }

    if (in > peakIntens/2)
      fwhm = 2 * (t - peakTth);
  }

  Gaussian gaussian;
  gaussian.setPeak(peakTth,peakIntens);
  gaussian.setFWHM(fwhm);

  FittingLevenbergMarquardt().fitWithoutCheck(gaussian,curve);

  return gaussian;
}

//------------------------------------------------------------------------------
}}
// eof

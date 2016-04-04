// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_fit_fitting.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_fit_fitting.h"
#include "core_fit_methods.h"
#include "core_fit_limits.h"
#include "types/core_type_curve.h"

namespace core { namespace fit {
//------------------------------------------------------------------------------

Polynomial fitBackground(core::Curve const& dgram, core::Ranges const& bgRanges, uint degree) {
  // clamp the degree - complexity of higher degree fits grows fast
  Polynomial bgPolynomial(qMin(degree,MAX_BACKGROUND_POLYNOMIAL_DEGREE));
  FittingLevenbergMarquardt().fitWithoutChecks(bgPolynomial,dgram.intersect(bgRanges));
  return bgPolynomial;
}

void fitPeak(PeakFunction& peakFunction, core::Curve const& curve_, core::Range const& range) {
  peakFunction.reset();

  core::Curve curve = curve_.intersect(range);
  if (curve.isEmpty()) return;

  if (!peakFunction.getGuessPeak().isValid()) { // calculate guesses
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

  FittingLevenbergMarquardt().fitWithoutChecks(peakFunction,curve);
}

//------------------------------------------------------------------------------
}}
// eof

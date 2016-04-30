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

Polynom fitPolynom(uint degree,
                   core::rcCurve curve, core::rcRanges ranges) {
  // clamp the degree - complexity of higher degree fits grows fast
  Polynom polynom(qMin(degree,MAX_POLYNOM_DEGREE));

  FittingLevenbergMarquardt().fitWithoutChecks(polynom, curve.intersect(ranges));

  return polynom;
}

void fit(PeakFunction& peakFunction,
                     core::rcCurve curve, core::rcRange range) {
  peakFunction.reset();

  core::Curve c = curve.intersect(range);
  if (c.isEmpty()) return;

  if (!peakFunction.guessedPeak().isValid()) { // calculate guesses
    uint peakIndex  = c.maxYindex();
    auto peakTth    = c.x(peakIndex);
    auto peakIntens = c.y(peakIndex);

    // half-maximum indices
    uint hmi1 = peakIndex, hmi2 = peakIndex;

    // left
    for (uint i=peakIndex; i-- > 0; ) {
      hmi1 = i;
      if (c.y(i) < peakIntens/2) break;
    }

    // right
    for (uint i=peakIndex, iCnt=c.count(); i < iCnt; ++i) {
      hmi2 = i;
      if (c.y(i) < peakIntens/2) break;
    }

    peakFunction.setGuessedPeak(XY(peakTth,peakIntens));
    peakFunction.setGuessedFWHM(c.x(hmi2) - c.x(hmi1));
  }

  FittingLevenbergMarquardt().fitWithoutChecks(peakFunction,c);
}

//------------------------------------------------------------------------------
}}
// eof

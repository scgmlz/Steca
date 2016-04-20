// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_reflection_info.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_reflection_info.h"
#include "core_session.h" // TODO OUT
#include "core_fit_fitting.h"
#include "types/core_type_matrix.h"
#include "types/core_type_curve.h"
#include <qmath.h>

namespace core{
//------------------------------------------------------------------------------

ReflectionInfo::ReflectionInfo() {
}

ReflectionInfo::ReflectionInfo(qreal const alpha,
                               qreal const beta,
                               rcRange gammaRange,
                               rcXY  peakPosition,
                               qreal const peakFWHM)
:  alpha_(alpha)
  ,beta_(beta)
  ,gammaRange_(gammaRange)
  ,peakPosition_(peakPosition)
  ,peakFWHM_(peakFWHM)
{
}

// Calculates the polefigure coordinates alpha and beta with regards to
// sample orientation and diffraction angles.
/*static*/ void calculateAlphaBeta(qreal omgDet, qreal phiDet, qreal chiDet, qreal tthRef, qreal gammaRef,
                        qreal& alpha, qreal& beta) {
  omgDet   = deg2Rad(omgDet);
  phiDet   = deg2Rad(phiDet);
  chiDet   = deg2Rad(chiDet);
  tthRef   = deg2Rad(tthRef);
  gammaRef = deg2Rad(gammaRef);

  // REVIEW

  // Note that the rotations here do not correspond to C. Randau's dissertation.
  // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
  const vector3d rotated =   matrix3d::rotationCWz (phiDet)
                           * matrix3d::rotationCWx (chiDet)
                           * matrix3d::rotationCWz (omgDet)
                           * matrix3d::rotationCWx (gammaRef)
                           * matrix3d::rotationCCWz(tthRef / 2)
                           * vector3d(0,1,0);
  alpha = acos(rotated._2);
  beta  = atan2(rotated._0, rotated._1);

  // Mirror angles.
  if (alpha > M_PI / 2) {
    alpha = qAbs(alpha - M_PI);
    beta += beta < 0 ? M_PI : -M_PI;
  }

  // Keep beta between 0 and 2pi.
  if (beta < 0)
    beta += 2 * M_PI;

  alpha = rad2Deg(alpha);
  beta  = rad2Deg(beta);
}

ReflectionInfo ReflectionInfo::make(
    rcSession session,
    rcDatasets /*datasets*/, rcDataset dataset,
    Reflection const& reflection,
    rcRange rgeTth, rcRange gammaSector)
{
  shp_Lens lens = session.lens(dataset, true, true, session.norm());
  Curve gammaCutCurve = lens->makeCurve(gammaSector,rgeTth);
/* TODO ask Antti - take fitted background (all picture)?
  const fit::Polynomial sectorBg
    = fit::fitBackground(gammaCutCurve,
                         session.getBgRanges(),
                         session.getBgPolynomialDegree());
  gammaCutCurve = gammaCutCurve.subtract(sectorBg);
*/
  QScopedPointer<fit::PeakFunction> peakFunction(reflection.peakFunction().clone());
  fit::fit(*peakFunction,
               gammaCutCurve,
               reflection.range());

  qreal alpha, beta;
  calculateAlphaBeta(dataset.omg(), dataset.phi(), dataset.omg(),
    reflection.range().center(),
    gammaSector.center(),
    alpha,
    beta
  );

  return ReflectionInfo(alpha,
                        beta,
                        gammaSector,
                        peakFunction->fittedPeak(),
                        peakFunction->fittedFWHM());
}

//------------------------------------------------------------------------------
}

// eof

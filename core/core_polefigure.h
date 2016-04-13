// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_polefigure.h
//! @brief     Pole figure
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_POLEFIGURE_H
#define CORE_POLEFIGURE_H

#include "types/core_types_fwd.h"
#include "types/core_type_range.h"
#include "core_reflection_info.h"

namespace core {
//------------------------------------------------------------------------------

class Session;

class Polefigure final {
public:
  const static int NUM_BETAS = 360 / 10; // Beta bin width 10 degrees.

public:
  Polefigure(rcSession,
             rcDatasets,
             Reflection const&,
             qreal alphaStep_,
             qreal betaStep_,
             rcRange rgeTth,
             Range gammaRange = Range()); // If no range specified, use the full range specified by the cuts.

  void generate(qreal centerRadius,
                qreal centerSearchRadius,
                qreal intensityTreshold,
                qreal searchRadius = qQNaN());
private:
  qreal alphaStep_;
  qreal betaStep_;
  QVector<qreal> FWHMs_;
  QVector<XY> peakPositions_;
  QVector<QVector<ReflectionInfo>> reflectionInfos_;
  Reflection const& reflection_;

  void searchPoints(qreal const alpha,
                    qreal const beta,
                    qreal const radius,
                    QList<qreal> &peakOffsets,
                    QList<qreal> &peakHeights,
                    QList<qreal> &FWHMs_) const;
  void searchPointsInAllQuadrants(qreal const alpha,
                                  qreal const beta,
                                  qreal const searchRadius,
                                  qreal &peakOffset,
                                  qreal &peakHeight,
                                  qreal &peakFWHM) const;
};
//------------------------------------------------------------------------------
}
#endif // CORE_POLEFIGURE_H

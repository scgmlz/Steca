#ifndef CORE_POLEFIGURE_H
#define CORE_POLEFIGURE_H

#include "types/core_types_fwd.h"
#include "types/core_type_range.h"
#include "core_reflection_info.h"

namespace core {

class Reflection;
class Session;

class Polefigure final {
public:
  const static int NUM_BETAS = 360 / 10; // Beta bin width 10 degrees.

public:
  Polefigure(Session &session,
             shp_File file,
             Reflection const& reflection,
             qreal alphaStep,
             qreal betaStep,
             Range gammaRange = Range()); // If no range specified, use the full range specified by the cuts.

  void generate(qreal centerRadius,
                qreal centerSearchRadius,
                qreal intensityTreshold,
                qreal searchRadius = qQNaN());
private:
  qreal alphaStep;
  qreal betaStep;
  QVector<qreal> FWHMs;
  QVector<XY> peakPositions;
  QVector<QVector<ReflectionInfo>> reflectionInfos;
  Reflection const* reflection;

  void searchPoints(qreal const alpha,
                    qreal const beta,
                    qreal const radius,
                    QList<qreal> &peakOffsets,
                    QList<qreal> &peakHeights,
                    QList<qreal> &FWHMs) const;
  void searchPointsInAllQuadrants(qreal const alpha,
                                  qreal const beta,
                                  qreal const searchRadius,
                                  qreal &peakOffset,
                                  qreal &peakHeight,
                                  qreal &peakFWHM) const;

};

}
#endif

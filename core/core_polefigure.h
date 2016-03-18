#ifndef CORE_POLEFIGURE_H
#define CORE_POLEFIGURE_H

#include "core_file.h"
#include "core_types.h"

namespace core {

class Reflection;
class Session;

class Polefigure final {
public:
  struct Point final {
    Point() = default;
    Point(qreal const alpha_,
          qreal const beta_,
          XY const& position,
          qreal const FWHM);

    qreal alpha;
    qreal beta;
    XY peakPosition;
    qreal peakFWHM;
  };

public:
  const static int NUM_BETAS = 360 / 10; // Beta bin width 10 degrees.

  static Point makePoint(Session const& session,
                         Dataset const& dataset,
                         shp_LensSystem lenses,
                         Reflection const& reflection,
                         Range const& gammaStripe);
public:
  Polefigure(Session &session,
             shp_File file,
             Reflection const& reflection_,
             qreal const alphaStep_,
             qreal const betaStep_,
             Range gammaRge = Range());

  void generate(qreal const centerRadius,
                qreal const centerSearchRadius,
                qreal const intensityTreshold,
                qreal const searchRadius = qQNaN());
private:
  qreal alphaStep;
  qreal betaStep;
  QVector<qreal> FWHMs;
  QVector<XY> peakPositions;
  QVector<QVector<Point>> points;
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
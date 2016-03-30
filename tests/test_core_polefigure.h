#ifndef TEST_CORE_POLEFIGURE_H
#define TEST_CORE_POLEFIGURE_H

#include "core_polefigure.h"

#include <algorithm>
#include <cmath>
#include <QLinkedList>
#include <QtMath>
#include <QtTest/QtTest>

namespace Quadrant {
  enum Quadrant {
    NORTHEAST,
    SOUTHEAST,
    SOUTHWEST,
    NORTHWEST,
    MAX_QUADRANTS
  };
}

namespace core {
  template<typename Container>
  qreal inverseDistanceWeighing(Container const& distances,
                                Container const& values);

  typedef QVector<qreal> TestContainer;
  template<TestContainer *container>
  qreal inverseDistanceWeighing(TestContainer const& distances,
                                TestContainer const& values);

  qreal calculateDeltaBeta(qreal beta1, qreal beta2) noexcept;
  qreal angle(qreal alpha1, qreal alpha2, qreal deltaBeta) noexcept;

  bool inQuadrant(int quadrant, qreal deltaAlpha, qreal deltaBeta) noexcept;

  Quadrant::Quadrant remapQuadrant(Quadrant::Quadrant const Q);

  core::Range gammaRangeAt(core::shp_LensSystem lenses, qreal const tth);

}

class TestCorePolefigure : public QObject {
  Q_OBJECT

private slots:
  void testPolefigure();
  void testInQuadrant();
  void testGamaRange();
  void testInverseDistanceWeighing();

};

#endif // TEST_CORE_POLEFIGURE_H

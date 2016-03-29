#ifndef TEST_CORE_POLEFIGURE_H
#define TEST_CORE_POLEFIGURE_H

#include "core_polefigure.h"

#include <algorithm>
#include <cmath>
#include <Eigen/Core>
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
  
  qreal calculateDeltaBeta(const qreal beta1, const qreal beta2) noexcept;
  qreal angle(const qreal alpha1, const qreal alpha2,
            const qreal deltaBeta) noexcept;
                     
  bool inQuadrant(const int quadrant,
                const qreal deltaAlpha, const qreal deltaBeta) noexcept;
                   
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

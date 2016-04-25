#ifndef TEST_CORE_POLEFIGURE_H
#define TEST_CORE_POLEFIGURE_H

#include "core_polefigure.h"
#include "core_session.h"

#include <algorithm>
#include <QLinkedList>
#include <qmath.h>
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
  void calculateAlphaBeta(rcDataset  dataset,
                            qreal tth, qreal gamma,
                            qreal& alpha, qreal& beta);
  
  namespace pole { 
    qreal angle(qreal alpha1, qreal alpha2, qreal deltaBeta);
    
    bool inRadius(qreal alpha, qreal beta,
              qreal centerAlpha, qreal centerBeta,
              qreal radius);
              
    template<typename Container>
    qreal inverseDistanceWeighing(Container const& distances,
                                  Container const& values);

    void inverseDistanceWeighing(qreal_vec const& distances,
                             QVector<ReflectionInfo const*> const& infos,
                             ReflectionInfo& out);
    
    qreal calculateDeltaBeta(qreal beta1, qreal beta2);
    
    bool inQuadrant(int quadrant, qreal deltaAlpha, qreal deltaBeta);
    
    Quadrant::Quadrant remapQuadrant(Quadrant::Quadrant const Q);
    
    core::Range gammaRangeAt(core::shp_Lens lenses, qreal const tth);
    
    void searchInQuadrants(
      uint_vec const& quadrants,
      qreal alpha, qreal beta,
      qreal searchRadius,
      ReflectionInfos const& infos,
      QVector<ReflectionInfo const*> & foundInfos,
      qreal_vec & distances);
      
  }
}

class TestCorePolefigure : public QObject {
  Q_OBJECT

private slots:
  void testRelatedMethods();
  void testInQuadrant();
  void testInverseDistanceWeighing();
  void testSearchInQuadrants();
  void testCalcAlphaBeta();
};

#endif // TEST_CORE_POLEFIGURE_H

#include "test_core_polefigure.h"
REGISTER_TEST_SUITE(TestCorePolefigure)

namespace Quadrant {
  enum Quadrant {
    NORTHEAST,
    SOUTHEAST,
    SOUTHWEST,
    NORTHWEST,
    MAX_QUADRANTS
  };
}

#include "core_polefigure.h"
#include "core_session.h"

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

#include "core_polefigure.h"

#include "test_core_lens.h"
#include "core_dataset.h"
#include "types/core_type_geometry.h"
#include "test_helpers.h"
#include "types/core_type_matrix.h"

#include <algorithm>
#include <QLinkedList>
#include <qmath.h>

using namespace core;
using namespace pole;


void TestCorePolefigure::testRelatedMethods() {

  {
    QCOMPARE(M_PI,deg2rad(180));
    QCOMPARE(90.0,rad2deg(M_PI_2));

    for_i (720)
      QCOMPARE((qreal)i,rad2deg(deg2rad(i)));
  }

  {
    qreal phiDet = 0;
    qreal chiDet = 0;

    auto m1 = matrix3d ::rotationCWz(phiDet);
    matrix3d cmpMatrix(1, 0, 0,
                       0, 1, 0,
                       0, 0, 1);

    QCOMPARE(m1, cmpMatrix);

    auto m2 = core::matrix3d::rotationCWx(chiDet);
    QCOMPARE(m2, cmpMatrix);
  }

  {
    qreal beta1 = 90;
    qreal beta2 = 0;
    auto b = calculateDeltaBeta(beta1,beta2);
    QCOMPARE(b,beta1);
  }

  {
    qreal angle1 = 90;
    qreal angle2 = 90;
    qreal angle3 = 0;
    auto a = angle(angle1,angle2,angle3);
    QCOMPARE(a,0.0);
  }

  {
    qreal alpha = 50;
    qreal beta = 40;
    qreal centerAlpha = 90;
    qreal centerBeta = 0;
    qreal radius = 90;

    QVERIFY(inRadius(alpha,beta,centerAlpha,centerBeta,radius));
  }

}

void TestCorePolefigure::testInQuadrant() {
  qreal deltaAlpha;
  qreal deltaBeta;

  {
    deltaAlpha = 0; deltaBeta = 0;

    QVERIFY(inQuadrant(Quadrant::NORTHEAST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::NORTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHEAST,deltaAlpha,deltaBeta));
  }

  {
    deltaAlpha = -1; deltaBeta = 0;
    QVERIFY(!inQuadrant(Quadrant::NORTHEAST,deltaAlpha,deltaBeta));
    QVERIFY(inQuadrant(Quadrant::NORTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHEAST,deltaAlpha,deltaBeta));
  }

  {
    deltaAlpha = -1; deltaBeta = -1;
    QVERIFY(!inQuadrant(Quadrant::NORTHEAST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::NORTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(inQuadrant(Quadrant::SOUTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHEAST,deltaAlpha,deltaBeta));
  }

  {
    deltaAlpha = 0; deltaBeta = -1;
    QVERIFY(!inQuadrant(Quadrant::NORTHEAST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::NORTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(inQuadrant(Quadrant::SOUTHEAST,deltaAlpha,deltaBeta));
  }

}

void TestCorePolefigure::testInverseDistanceWeighing() {
  qreal_vec distances;
  distances.append(2.0);
  distances.append(1.0);
  distances.append(1.0);
  distances.append(2.0);

  inten_t inten = 2;
  qreal tth     = 1;
  qreal fwhm    = 4;
  QVector<ReflectionInfo const*> infos;
  ReflectionInfo in;
  for_i(4) {
    in.inten_ = inten;
    in.tth_   = tth;
    in.fwhm_  = fwhm;
    infos.append(&in);
  }
  ReflectionInfo out;
  qreal tmp_height = 0;
  qreal tmp_offset = 0;
  qreal tmp_fwhm   = 0;
  inverseDistanceWeighing(distances,infos,out);
  for_i (Quadrant::MAX_QUADRANTS) {
    tmp_height += infos[0]->inten() * (qreal)1/distances[i];
    tmp_offset += infos[0]->tth()   * (qreal)1/distances[i];
    tmp_fwhm   += infos[0]->fwhm()  * (qreal)1/distances[i];
  }
  QCOMPARE(out.inten(),tmp_height/3);
  QCOMPARE(out.tth(),tmp_offset/3);
  QCOMPARE(out.fwhm(),tmp_fwhm/3);
}

void TestCorePolefigure::testSearchInQuadrants() {
  qreal alpha = 15, beta = 20, searchRadius = 5;
  QVector<ReflectionInfo const*> foundInfos;
  ReflectionInfos infos;
  ReflectionInfo in;
  in.alpha_ = 20;
  in.beta_ = 20; // inside of BETA_LIMIT
  infos.append(in);
  in.alpha_ = 180;
  in.beta_ = 180; // outside of BETA_LIMIT
  infos.append(in);

  qreal_vec distances;

  searchInQuadrants(uint_vec(Quadrant::SOUTHWEST,0),alpha,beta,
    searchRadius,infos,foundInfos,distances);

  QCOMPARE(foundInfos.size(),2);
  QCOMPARE(foundInfos[0]->alpha(),infos[0].alpha());
  QCOMPARE(foundInfos[1]->alpha(),infos[0].alpha());

}

void TestCorePolefigure::testCalcAlphaBeta() {
  qreal alpha;
  qreal beta;
  TestHelpers testHelper;
  QVector<qreal> angles;
  qreal mon = 100, deltaTime = 8, tth = 0, gamma = 0;
  Session s;
  {
    angles.fill(0,10);
    auto const dataset = testHelper.testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gamma, alpha,beta);

    QCOMPARE(alpha,rad2deg(acos(0)));
    QCOMPARE(beta,rad2deg(atan2(0,1)));
  }

  {
    angles.fill(0,10);
    angles[3] = rad2deg(M_PI/2);

    auto const dataset = testHelper.testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gamma, alpha,beta);
    //rotated = -1,0,0
    QCOMPARE(alpha,rad2deg(acos(0)));
    QCOMPARE(beta,rad2deg(atan2(-1,0) + 2 * M_PI));
  }

  {
    angles.fill(0,10);
    angles[5] = rad2deg(M_PI/2);

    auto const dataset = testHelper.testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gamma, alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,rad2deg(acos(0)));
    QCOMPARE(beta,rad2deg(atan2(-1,0) + 2*M_PI));
  }

  {
    angles.fill(0,10);
    angles[6] = rad2deg(M_PI/2);

    auto const dataset = testHelper.testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gamma, alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,rad2deg(acos(1)));
    QCOMPARE(beta,rad2deg(atan2(0,0)));
  }

  {
    angles.fill(0,10);
    qreal tthRef = 90.0;
    auto const dataset = testHelper.testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tthRef, gamma, alpha,beta);
    // rotated = 0,-1,0
    QCOMPARE(alpha,rad2deg(acos(0)));
    QCOMPARE(beta,rad2deg(atan2(sin(M_PI/4),cos(M_PI/4))));
  }

  {
    angles.fill(0,10);
    qreal gammaRef = 90;

    auto const dataset = testHelper.testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gammaRef, alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,rad2deg(acos(1)));
    QCOMPARE(beta,rad2deg(atan2(0,0)));
  }

  {
    angles.fill(0,10);
    angles[0] = rad2deg(M_PI);
    angles[1] = rad2deg(M_PI);
    angles[2] = rad2deg(M_PI);

    qreal const tthRef = 4*rad2deg(M_PI);
    qreal const gammaRef = rad2deg(M_PI);

    auto const dataset = testHelper.testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tthRef, gammaRef, alpha,beta);

    QCOMPARE(alpha,rad2deg(acos(0)));
  }
}

// eof

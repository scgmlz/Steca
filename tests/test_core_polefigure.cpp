#include "test_core_polefigure.h"
#include "core_polefigure.h"
#include "core_polefigure.cpp"
#include "test_core_lens.h"
#include "core_dataset.h"

#include <algorithm>
#include <cmath>
#include <Eigen/Core>
#include <QLinkedList>
#include <QtMath>

using namespace core;

#define TEST_DATA \
  int width             = 2;\
  int height            = 3;\
  intens_t inten        = 42.0f;\
  intens_t specialInten = 150.0f;\
  int posIntensArray    = 0;\
  qreal gamma           = 6.7;\
  qreal gammaSpecial    = 44.0;\
  qreal tth             = 4.2;\
  qreal tthSpecial      = 88.0;\
  int anglePosX         = 1;\
  int anglePosY         = 2;\
  TestCoreLens test;\

void TestPolefigure::testPolefigure() {

}

void TestPolefigure::testRotation() {
  {
    const qreal phiDet = 0;
    const qreal chiDet = 0;


    auto m1 = rotationCWz(phiDet);
    matrix3d cmpMatrix;
    cmpMatrix << 1 , 0 , 0,
        0 , 1 , 0,
        0 , 0 , 1;

    QCOMPARE(m1, cmpMatrix);

    auto m2 = rotationCWx(chiDet);
    QCOMPARE(m2, cmpMatrix);
  }

}

#define TEST_ANGLES(val1,val2,val3,val4,val5) \
  const qreal omgDet = val1;   \
  const qreal phiDet = val2;   \
  const qreal chiDet = val3;   \
  const qreal tthRef = val4;   \
  const qreal gammaRef = val5; \

void TestPolefigure::testCalcAlphaBeta() {
  {
    TEST_ANGLES(0,0,0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);

    QCOMPARE(alpha,acos(0));
    QCOMPARE(beta,atan2(0,1));
  }

  {
    TEST_ANGLES(M_PI/2,0,0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,acos(0));
    QCOMPARE(beta,atan2(-1,0) + 2 * M_PI);
  }

  {
    TEST_ANGLES(0,(M_PI/2),0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,acos(0));
    QCOMPARE(beta,atan2(-1,0) + 2*M_PI);
  }

  {
    TEST_ANGLES(0,0,(M_PI/2),0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,acos(1));
    QCOMPARE(beta,atan2(0,0));
  }

  {
    TEST_ANGLES(0,0,0,(M_PI/2),0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,-1,0
    QCOMPARE(alpha,acos(0));
    QCOMPARE(beta,atan2(sin(M_PI/4),cos(M_PI/4)));
  }

  {
    TEST_ANGLES(0,0,0,0,(M_PI/2))

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,acos(1));
    QCOMPARE(beta,atan2(0,0));
  }

  {
    const qreal omgDet = M_PI;
    const qreal phiDet = M_PI;
    const qreal chiDet = M_PI;
    const qreal tthRef = 4*M_PI;
    const qreal gammaRef = M_PI;

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);

    QCOMPARE(alpha,acos(0));
    // QCOMPARE(beta,atan2(0,1));
  }

}

void TestPolefigure::testInQuadrant() {
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

void TestPolefigure::testGamaRange() {
  TEST_DATA
  Dataset dataset = test.testDataset(width,height,inten,specialInten,posIntensArray);
  AngleMapArray angleMapArray = test.testAngleMapArray(gamma,gammaSpecial,
                                                  tth,tthSpecial,
                                                  width,height,
                                                  anglePosX,anglePosY);

  qreal tthNext   = 3.0;
  qreal gammaNext = 8.0;
  angleMapArray.setAt(1,0,DiffractionAngles(gammaNext,tthNext));
  auto lensSystem = makeLensSystem(dataset, angleMapArray);
  qreal testTth = 4.0;
  Range extendRange = Range(gammaNext);
  auto range = gammaRange(lensSystem,testTth);

  QCOMPARE(range.max, extendRange.max);
  QCOMPARE(range.max, extendRange.min);


}

void TestPolefigure::testInverseDistanceWeighing() {

  TestContainer distances;
  distances.append(1.0);
  distances.append(2.0);
  distances.append(3.0);
  distances.append(4.0);

  TestContainer values;
  values.append(1.0);
  values.append(2.0);
  values.append(3.0);
  values.append(4.0);

  qreal in = inverseDistanceWeighing(distances,values);
  qreal cmp = (qreal)4.0/(qreal)(25.0/12.0);
  QCOMPARE(in,cmp);

}

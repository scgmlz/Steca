#include "test_core_polefigure.h"
#include "core_polefigure.h"
#include "core_polefigure.cpp"

#include <algorithm>
#include <cmath>
#include <Eigen/Core>
#include <QLinkedList>
#include <QtMath>

using namespace core;

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

#define TEST_DATA(val1,val2,val3,val4,val5) \
  const qreal omgDet = val1;   \
  const qreal phiDet = val2;   \
  const qreal chiDet = val3;   \
  const qreal tthRef = val4;   \
  const qreal gammaRef = val5; \

void TestPolefigure::testCalcAlphaBeta() {
  {
    TEST_DATA(0,0,0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);

    QCOMPARE(alpha,acos(0));
    QCOMPARE(beta,atan2(0,1));
  }

  {
    TEST_DATA(M_PI/2,0,0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,acos(0));
    QCOMPARE(beta,atan2(-1,0) + 2 * M_PI);
  }

  {
    TEST_DATA(0,(M_PI/2),0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,acos(0));
    QCOMPARE(beta,atan2(-1,0) + 2*M_PI);
  }

  {
    TEST_DATA(0,0,(M_PI/2),0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,acos(1));
    QCOMPARE(beta,atan2(0,0));
  }

  {
    TEST_DATA(0,0,0,(M_PI/2),0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,-1,0
    QCOMPARE(alpha,acos(0));
    QCOMPARE(beta,atan2(sin(M_PI/4),cos(M_PI/4)));
  }

  {
    TEST_DATA(0,0,0,0,(M_PI/2))

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

void TestPolefigure::testRemapQuadrant() {

}

void TestPolefigure::testGamaRange() {
  shp_LensSystem lens;


}

void TestPolefigure::inverseDistanceWeighing() {

}

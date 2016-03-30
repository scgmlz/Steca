#include "test_core_dataset.h"
#include "core_types.h"

#include <cmath>

namespace core {

matrix3d rotationCWx(qreal angle);
matrix3d rotationCWz(qreal angle);
matrix3d rotationCCWz(qreal angle);

void calculateAlphaBeta(qreal omgDet, qreal phiDet,
                        qreal chiDet,
                        qreal tthRef, qreal gammaRef,
                        qreal& alpha, qreal& beta);
}

using namespace core;

void TestCoreDataset::testRotation() {
  {
    qreal phiDet = 0;
    qreal chiDet = 0;


    auto m1 = rotationCWz(phiDet);
    matrix3d cmpMatrix(1, 0, 0,
                       0, 1, 0,
                       0, 0, 1);

    QCOMPARE(m1, cmpMatrix);

    auto m2 = rotationCWx(chiDet);
    QCOMPARE(m2, cmpMatrix);
  }

}

#define TEST_ANGLES(val1,val2,val3,val4,val5) \
  qreal omgDet = val1;   \
  qreal phiDet = val2;   \
  qreal chiDet = val3;   \
  qreal tthRef = val4;   \
  qreal gammaRef = val5; \

void TestCoreDataset::testCalcAlphaBeta() {
  {
    TEST_ANGLES(0,0,0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);

    QCOMPARE(alpha,radToDeg(acos(0)));
    QCOMPARE(beta,radToDeg(atan2(0,1)));
  }

  {
    TEST_ANGLES(radToDeg(M_PI/2),0,0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,radToDeg(acos(0)));
    QCOMPARE(beta,radToDeg(atan2(-1,0) + 2 * M_PI));
  }

  {
    TEST_ANGLES(0,radToDeg(M_PI/2),0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,radToDeg(acos(0)));
    QCOMPARE(beta,radToDeg(atan2(-1,0) + 2*M_PI));
  }

  {
    TEST_ANGLES(0,0,radToDeg(M_PI/2),0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,radToDeg(acos(1)));
    QCOMPARE(beta,radToDeg(atan2(0,0)));
  }

  {
    TEST_ANGLES(0,0,0,radToDeg(M_PI/2),0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,-1,0
    QCOMPARE(alpha,radToDeg(acos(0)));
    QCOMPARE(beta,radToDeg(atan2(sin(M_PI/4),cos(M_PI/4))));
  }

  {
    TEST_ANGLES(0,0,0,0,radToDeg(M_PI/2))

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,radToDeg(acos(1)));
    QCOMPARE(beta,radToDeg(atan2(0,0)));
  }

  {
    qreal const omgDet = radToDeg(M_PI);
    qreal const phiDet = radToDeg(M_PI);
    qreal const chiDet = radToDeg(M_PI);
    qreal const tthRef = 4*radToDeg(M_PI);
    qreal const gammaRef = radToDeg(M_PI);

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);

    QCOMPARE(alpha,radToDeg(acos(0)));
  }

}


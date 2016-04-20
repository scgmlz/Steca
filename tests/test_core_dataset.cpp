#include "test_core_dataset.h"

#include "core_dataset.h"
#include "core_reflection.h"
#include "types/core_type_matrix.h"

using namespace core;

void TestCoreDataset::testRotation() {
  {
    qreal phiDet = 0;
    qreal chiDet = 0;

    auto m1 = matrix3d ::rotationCWz(phiDet);
    matrix3d cmpMatrix(1, 0, 0,
                       0, 1, 0,
                       0, 0, 1);

    QCOMPARE(m1, cmpMatrix);

    auto m2 = matrix3d::rotationCWx(chiDet);
    QCOMPARE(m2, cmpMatrix);
  }
}

#define TEST_ANGLES(val1,val2,val3,val4,val5) \
  qreal omgDet = val1;   \
  qreal phiDet = val2;   \
  qreal chiDet = val3;   \
  qreal tthRef = val4;   \
  qreal gammaRef = val5; \

//namespace core {
//void calculateAlphaBeta(qreal, qreal, qreal, qreal, qreal, qreal&, qreal&);
//}
/*
void TestCoreDataset::testCalcAlphaBeta() {
  {
    TEST_ANGLES(0,0,0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);

    QCOMPARE(alpha,rad2deg(acos(0)));
    QCOMPARE(beta,rad2deg(atan2(0,1)));
  }

  {
    TEST_ANGLES(rad2deg(M_PI/2),0,0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,rad2deg(acos(0)));
    QCOMPARE(beta,rad2deg(atan2(-1,0) + 2 * M_PI));
  }

  {
    TEST_ANGLES(0,rad2deg(M_PI/2),0,0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,rad2deg(acos(0)));
    QCOMPARE(beta,rad2deg(atan2(-1,0) + 2*M_PI));
  }

  {
    TEST_ANGLES(0,0,rad2deg(M_PI/2),0,0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,rad2deg(acos(1)));
    QCOMPARE(beta,rad2deg(atan2(0,0)));
  }

  {
    TEST_ANGLES(0,0,0,rad2deg(M_PI/2),0)

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,-1,0
    QCOMPARE(alpha,rad2deg(acos(0)));
    QCOMPARE(beta,rad2deg(atan2(sin(M_PI/4),cos(M_PI/4))));
  }

  {
    TEST_ANGLES(0,0,0,0,rad2deg(M_PI/2))

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,rad2deg(acos(1)));
    QCOMPARE(beta,rad2deg(atan2(0,0)));
  }

  {
    qreal const omgDet = rad2deg(M_PI);
    qreal const phiDet = rad2deg(M_PI);
    qreal const chiDet = rad2deg(M_PI);
    qreal const tthRef = 4*rad2deg(M_PI);
    qreal const gammaRef = rad2deg(M_PI);

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);

    QCOMPARE(alpha,rad2deg(acos(0)));
  }
}
*/


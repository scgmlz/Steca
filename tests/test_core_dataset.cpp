#include "test_core_dataset.h"
#include "core_types.h"

#include <Eigen/Core>
#include <cmath>

namespace core {
using matrix3d = Eigen::Matrix<qreal,3,3>;
using vector3d = Eigen::Matrix<qreal,3,1>;

matrix3d rotationCWx(const qreal angle);
matrix3d rotationCWz(const qreal angle);
matrix3d rotationCCWz(const qreal angle);

void calculateAlphaBeta(const qreal omgDet, const qreal phiDet,
                        const qreal chiDet,
                        const qreal tthRef, const qreal gammaRef,
                        qreal& alpha, qreal& beta);
}

using namespace core;

void TestCoreDataset::testRotation() {
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
    const qreal omgDet = radToDeg(M_PI);
    const qreal phiDet = radToDeg(M_PI);
    const qreal chiDet = radToDeg(M_PI);
    const qreal tthRef = 4*radToDeg(M_PI);
    const qreal gammaRef = radToDeg(M_PI);

    qreal alpha;
    qreal beta;

    calculateAlphaBeta(omgDet,phiDet,chiDet,tthRef,gammaRef,alpha,beta);

    QCOMPARE(alpha,radToDeg(acos(0)));
  }

}


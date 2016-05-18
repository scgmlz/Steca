#include "test_core_approx.h"
REGISTER_TEST_SUITE(TestCoreApprox)

#include "types/core_type_curve.h"
#include "core_fit_functions.h"
#include "core_fit_methods.h"

// check if two qreals are equal for wanted precision
#define QALMOST_COMPARE(a,b) QVERIFY(qAbs(a-b) < 1E-3)

void TestCoreApprox::testPolynom() {
  {
    core::Curve curve;
    curve.append(1,1);
    curve.append(2,2);

    core::fit::Polynom p(1);

    core::fit::FittingLevenbergMarquardt fm;
    QVERIFY(fm.fitWithoutChecks(p,curve));

    core::Range r(0,2);
    qreal average = p.avgY(r);

    QALMOST_COMPARE(average,1.0);
  }
}

void TestCoreApprox::testLinearLeastSquare() {
  { // simple 1st order function
    core::Curve c;
    for_i (3)
      c.append(i,i);

    core::fit::Polynom p(1);

    core::fit::FittingLinearLeastSquare fm;
    QVERIFY(fm.fitWithoutChecks(p,c));

    QALMOST_COMPARE(p.parameterAt(0).value(), 0.);
    QALMOST_COMPARE(p.parameterAt(1).value(), 1.);
  }

  { // 1st order function
    core::Curve c;
    c.append(1,4);
    c.append(2,6);
    c.append(3,8);

    core::fit::Polynom p(1);

    core::fit::FittingLinearLeastSquare fm;
    QVERIFY(fm.fitWithoutChecks(p,c));

    QALMOST_COMPARE(p.parameterAt(0).value(), 2.);
    QALMOST_COMPARE(p.parameterAt(1).value(), 2.);
  }

  { // 2nd order
    core::Curve c;
    c.append(1,11);
    c.append(2,20);
    c.append(3,33);

    core::fit::Polynom p(2);

    core::fit::FittingLinearLeastSquare fm;
    QVERIFY(fm.fitWithoutChecks(p,c));

    QALMOST_COMPARE(p.parameterAt(0).value(), 6.);
    QALMOST_COMPARE(p.parameterAt(1).value(), 3.);
    QALMOST_COMPARE(p.parameterAt(2).value(), 2.);
  }

  { // 3rd order
    core::Curve c;
    c.append(0,42);
    c.append(1,43.1);
    c.append(2,46.4);
    c.append(3,52.5);

    core::fit::Polynom p(3);

    core::fit::FittingLevenbergMarquardt fm;
    QVERIFY(fm.fitWithoutChecks(p,c));

    QALMOST_COMPARE(p.parameterAt(0).value(), 42.);
    QALMOST_COMPARE(p.parameterAt(1).value(), .2);
    QALMOST_COMPARE(p.parameterAt(2).value(), .8);
    QALMOST_COMPARE(p.parameterAt(3).value(), .1);
  }
}

void TestCoreApprox::testLevenbergMarquardt() {
  {
    core::Curve c;
    for_i (3) {
      c.append(i,i);
    }

    core::fit::Polynom p(1);
    core::fit::FittingLevenbergMarquardt fm;
    QVERIFY(fm.fitWithoutChecks(p,c));

    QALMOST_COMPARE(p.parameterAt(0).value(), 0.);
    QALMOST_COMPARE(p.parameterAt(1).value(), 1.);
  }

  { // 1st order function
    core::Curve c;
    c.append(1,4);
    c.append(2,6);
    c.append(3,8);

    core::fit::Polynom p(1);

    core::fit::FittingLevenbergMarquardt fm;
    QVERIFY(fm.fitWithoutChecks(p,c));

    QALMOST_COMPARE(p.parameterAt(0).value(), 2.);
    QALMOST_COMPARE(p.parameterAt(1).value(), 2.);
  }


  { // 2nd order
    core::Curve c;
    c.append(1,11);
    c.append(2,20);
    c.append(3,33);

    core::fit::Polynom p(2);

    core::fit::FittingLevenbergMarquardt fm;
    QVERIFY(fm.fitWithoutChecks(p,c));

    QALMOST_COMPARE(p.parameterAt(0).value(), 6.);
    QALMOST_COMPARE(p.parameterAt(1).value(), 3.);
    QALMOST_COMPARE(p.parameterAt(2).value(), 2.);
  }

  { // 3rd order
    core::Curve c;
    c.append(0,42);
    c.append(1,43.1);
    c.append(2,46.4);
    c.append(3,52.5);

    core::fit::Polynom p(3);

    core::fit::FittingLevenbergMarquardt fm;
    QVERIFY(fm.fitWithoutChecks(p,c));

    QALMOST_COMPARE(p.parameterAt(0).value(), 42.);
    QALMOST_COMPARE(p.parameterAt(1).value(), .2);
    QALMOST_COMPARE(p.parameterAt(2).value(), .8);
    QALMOST_COMPARE(p.parameterAt(3).value(), .1);
  }
  { // 4th order
    core::Curve c;
    c.append(0,42);
    c.append(1,53.8);
    c.append(2,98.4);
    c.append(3,211.8);
    c.append(4,437.2);

    core::fit::Polynom p(4);

    core::fit::FittingLevenbergMarquardt fm;
    QVERIFY(fm.fitWithoutChecks(p,c));

    QALMOST_COMPARE(p.parameterAt(0).value(), 42.);
    QALMOST_COMPARE(p.parameterAt(1).value(), 5.6);
    QALMOST_COMPARE(p.parameterAt(2).value(), 1.7);
    QALMOST_COMPARE(p.parameterAt(3).value(), 4.2);
    QALMOST_COMPARE(p.parameterAt(4).value(), 0.3);
  }
}

// eof

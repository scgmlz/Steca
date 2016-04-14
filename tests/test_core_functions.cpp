#include "test_core_functions.h"

#include "types/core_type_curve.h"
#include "core_fit_functions.h"
#include "core_fit_methods.h"

// check if two qreals are equal for wanted precision
#define QALMOST_COMPARE(a,b) QVERIFY(qAbs(a-b) < 1E-3)

void TestCoreApprox::testPolynomial() {
    { // TODO move test test_core_polynomial or rename test class
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve curve;
    curve.append(1,1);
    curve.append(2,2);
    core::fit::Polynomial p(1);
    fm.fitWithoutChecks(p,curve);
    core::Range r(0,2);
    qreal average = p.calAverageValue(r);

    QALMOST_COMPARE(average,1.0);
  }
}

void TestCoreApprox::testApproxFunction() {
//approximation using LinearLeastSquare
  { // simple 1.Order Function
    core::fit::FittingLinearLeastSquare fm;
    core::Curve c;

    for_i (3) {
      c.append(i,i);
    }

    core::fit::Polynomial p(1);
    QVERIFY(fm.fitWithoutChecks(p,c));   // test if fitting succesfull

    //test if calculated parameters are aproximatly the same
    core::fit::Function::Parameter parameter = p.parameterAt(0);
    QALMOST_COMPARE(parameter.value(),0.);
    parameter = p.parameterAt(1);
    QALMOST_COMPARE(parameter.value(),1.);
  }

  { // 1.Order Function
    core::fit::FittingLinearLeastSquare fm;
    core::Curve c;
    c.append(1,4);
    c.append(2,6);
    c.append(3,8);
    core::fit::Polynomial p(1);
    QVERIFY(fm.fitWithoutChecks(p,c));
    core::fit::Function::Parameter parameter = p.parameterAt(0);
    QALMOST_COMPARE(parameter.value(),2.);
    parameter = p.parameterAt(1);
    QALMOST_COMPARE(parameter.value(),2.);
  }

  {// 2.Order
    core::fit::FittingLinearLeastSquare fm;
    core::Curve c;
    c.append(1,11);
    c.append(2,20);
    c.append(3,33);
    core::fit::Polynomial p(2);
    QVERIFY(fm.fitWithoutChecks(p,c));
    core::fit::Function::Parameter parameter = p.parameterAt(0);
    QALMOST_COMPARE(parameter.value(),6.);
    parameter = p.parameterAt(1);
    QALMOST_COMPARE(parameter.value(),3.);
    parameter = p.parameterAt(2);
    QALMOST_COMPARE(parameter.value(),2.);
  }

  {// 3.Order
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(0,42);
    c.append(1,43.1);
    c.append(2,46.4);
    c.append(3,52.5);
    core::fit::Polynomial p(3);
    QVERIFY(fm.fitWithoutChecks(p,c));
    core::fit::Function::Parameter parameter = p.parameterAt(0);
    QALMOST_COMPARE(parameter.value(),42.);
    parameter = p.parameterAt(1);
    QALMOST_COMPARE(parameter.value(),0.2);
    parameter = p.parameterAt(2);
    QALMOST_COMPARE(parameter.value(),0.8);
    parameter = p.parameterAt(3);
    QALMOST_COMPARE(parameter.value(),0.1);
  }

//Approximation using LevenbergMarquardt
  {
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    for_i (3) {
      c.append(i,i);
    }
    core::fit::Polynomial p(1);
    QVERIFY(fm.fitWithoutChecks(p,c));//test if fitting succesfull
    //test if calculated parameters are aproximatly the same
    core::fit::Function::Parameter parameter = p.parameterAt(0);
    QALMOST_COMPARE(parameter.value(),0.);
    parameter = p.parameterAt(1);
    QALMOST_COMPARE(parameter.value(),1.);
  }

  { // 1.Order Function
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(1,4);
    c.append(2,6);
    c.append(3,8);
    core::fit::Polynomial p(1);
    QVERIFY(fm.fitWithoutChecks(p,c));
    core::fit::Function::Parameter parameter = p.parameterAt(0);
    QALMOST_COMPARE(parameter.value(),2.);
    parameter = p.parameterAt(1);
    QALMOST_COMPARE(parameter.value(),2.);
  }


  {// 2.Order
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(1,11);
    c.append(2,20);
    c.append(3,33);
    core::fit::Polynomial p(2);
    QVERIFY(fm.fitWithoutChecks(p,c));
    core::fit::Function::Parameter parameter = p.parameterAt(0);
    QALMOST_COMPARE(parameter.value(),6.);
    parameter = p.parameterAt(1);
    QALMOST_COMPARE(parameter.value(),3.);
    parameter = p.parameterAt(2);
    QALMOST_COMPARE(parameter.value(),2.);
  }

  {// 3.Order
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(0,42);
    c.append(1,43.1);
    c.append(2,46.4);
    c.append(3,52.5);
    core::fit::Polynomial p(3);
    QVERIFY(fm.fitWithoutChecks(p,c));
    core::fit::Function::Parameter parameter = p.parameterAt(0);
    QALMOST_COMPARE(parameter.value(),42.);
    parameter = p.parameterAt(1);
    QALMOST_COMPARE(parameter.value(),0.2);
    parameter = p.parameterAt(2);
    QALMOST_COMPARE(parameter.value(),0.8);
    parameter = p.parameterAt(3);
    QALMOST_COMPARE(parameter.value(),0.1);
  }
  { // 4.Order
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(0,42);
    c.append(1,53.8);
    c.append(2,98.4);
    c.append(3,211.8);
    c.append(4,437.2);
    core::fit::Polynomial p(4);
    QVERIFY(fm.fitWithoutChecks(p,c));
    core::fit::Function::Parameter parameter = p.parameterAt(0);
    QALMOST_COMPARE(parameter.value(),42.);
    parameter = p.parameterAt(1);
    QALMOST_COMPARE(parameter.value(),5.6);
    parameter = p.parameterAt(2);
    QALMOST_COMPARE(parameter.value(),1.7);
    parameter = p.parameterAt(3);
    QALMOST_COMPARE(parameter.value(),4.2);
    parameter = p.parameterAt(4);
    QALMOST_COMPARE(parameter.value(),0.3);
  }
}

#include "test_approx.h"

#include <core_fit_functions.h>
#include <core_fit_methods.h>

// check if two qreals are equal for wanted precision
#define QALMOST_COMPARE(a,b) QVERIFY(qAbs(a-b) < 1E-3)

void TestApprox::testApproxFunction() {
//approximation using LinearLeastSquare
  { // simple 1.Order Function
    core::fit::FittingLinearLeastSquare fm;
    core::Curve c;

    for_i(3) {
      c.append(i,i);
    }

    core::fit::Polynomial p(1);
    QVERIFY(fm.fitWithoutCheck(p,c));   // test if fitting succesfull

    //test if calculated parameters are aproximatly the same
    core::fit::Function::Parameter parameter = p.getParameter(0);
    QALMOST_COMPARE(parameter.getValue(),0.);
    parameter = p.getParameter(1);
    QALMOST_COMPARE(parameter.getValue(),1.);
  }

  { // 1.Order Function
    core::fit::FittingLinearLeastSquare fm;
    core::Curve c;
    c.append(1,4);
    c.append(2,6);
    c.append(3,8);
    core::fit::Polynomial p(1);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::fit::Function::Parameter parameter = p.getParameter(0);
    QALMOST_COMPARE(parameter.getValue(),2.);
    parameter = p.getParameter(1);
    QALMOST_COMPARE(parameter.getValue(),2.);
  }

  {// 2.Order
    core::fit::FittingLinearLeastSquare fm;
    core::Curve c;
    c.append(1,11);
    c.append(2,20);
    c.append(3,33);
    core::fit::Polynomial p(2);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::fit::Function::Parameter parameter = p.getParameter(0);
    QALMOST_COMPARE(parameter.getValue(),6.);
    parameter = p.getParameter(1);
    QALMOST_COMPARE(parameter.getValue(),3.);
    parameter = p.getParameter(2);
    QALMOST_COMPARE(parameter.getValue(),2.);
  }

  {// 3.Order
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(0,42);
    c.append(1,43.1);
    c.append(2,46.4);
    c.append(3,52.5);
    core::fit::Polynomial p(3);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::fit::Function::Parameter parameter = p.getParameter(0);
    QALMOST_COMPARE(parameter.getValue(),42.);
    parameter = p.getParameter(1);
    QALMOST_COMPARE(parameter.getValue(),0.2);
    parameter = p.getParameter(2);
    QALMOST_COMPARE(parameter.getValue(),0.8);
    parameter = p.getParameter(3);
    QALMOST_COMPARE(parameter.getValue(),0.1);
  }

//Approximation using LevenbergMarquardt
  {
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    for_i(3){
      c.append(i,i);
    }
    core::fit::Polynomial p(1);
    QVERIFY(fm.fitWithoutCheck(p,c));//test if fitting succesfull
    //test if calculated parameters are aproximatly the same
    core::fit::Function::Parameter parameter = p.getParameter(0);
    QALMOST_COMPARE(parameter.getValue(),0.);
    parameter = p.getParameter(1);
    QALMOST_COMPARE(parameter.getValue(),1.);
  }

  { // 1.Order Function
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(1,4);
    c.append(2,6);
    c.append(3,8);
    core::fit::Polynomial p(1);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::fit::Function::Parameter parameter = p.getParameter(0);
    QALMOST_COMPARE(parameter.getValue(),2.);
    parameter = p.getParameter(1);
    QALMOST_COMPARE(parameter.getValue(),2.);
  }


  {// 2.Order
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(1,11);
    c.append(2,20);
    c.append(3,33);
    core::fit::Polynomial p(2);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::fit::Function::Parameter parameter = p.getParameter(0);
    QALMOST_COMPARE(parameter.getValue(),6.);
    parameter = p.getParameter(1);
    QALMOST_COMPARE(parameter.getValue(),3.);
    parameter = p.getParameter(2);
    QALMOST_COMPARE(parameter.getValue(),2.);
  }

  {// 3.Order
    core::fit::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(0,42);
    c.append(1,43.1);
    c.append(2,46.4);
    c.append(3,52.5);
    core::fit::Polynomial p(3);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::fit::Function::Parameter parameter = p.getParameter(0);
    QALMOST_COMPARE(parameter.getValue(),42.);
    parameter = p.getParameter(1);
    QALMOST_COMPARE(parameter.getValue(),0.2);
    parameter = p.getParameter(2);
    QALMOST_COMPARE(parameter.getValue(),0.8);
    parameter = p.getParameter(3);
    QALMOST_COMPARE(parameter.getValue(),0.1);
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
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::fit::Function::Parameter parameter = p.getParameter(0);
    QALMOST_COMPARE(parameter.getValue(),42.);
    parameter = p.getParameter(1);
    QALMOST_COMPARE(parameter.getValue(),5.6);
    parameter = p.getParameter(2);
    QALMOST_COMPARE(parameter.getValue(),1.7);
    parameter = p.getParameter(3);
    QALMOST_COMPARE(parameter.getValue(),4.2);
    parameter = p.getParameter(4);
    QALMOST_COMPARE(parameter.getValue(),0.3);
  }
}


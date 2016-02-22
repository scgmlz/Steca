#include "test_approx.h"

#include <approx/approx_functions.h>
#include <approx/approx_methods.h>


#define EQUAL_WITH_PRECISION(a,b,precision) \

//check if two qreals are equal for wanted precision
bool TestApprox::nearlyEqual(qreal a, qreal b, qreal precision) {
  if(a<b) {
    qSwap(a,b);
  }
  if((a-b)<precision) {
      return true;
  }
  return false;
}

void TestApprox::testApproxFunction() {
//approximation using LinearLeastSquare
  { // simple 1.Order Function
    core::approx::FittingLinearLeastSquare fm;
    core::Curve c;
    for_i(3){
      c.append(i,i);
    }
    core::approx::Polynomial p(1);
    QVERIFY(fm.fitWithoutCheck(p,c));//test if fitting succesfull
    //test if calculated parameters are aproximatly the same
    core::approx::Function::Parameter parameter = p.getParameter(0);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(0),1E-3));
    parameter = p.getParameter(1);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(1),1E-3));
  }
  { // 1.Order Function
    core::approx::FittingLinearLeastSquare fm;
    core::Curve c;
    c.append(1,4);
    c.append(2,6);
    c.append(3,8);
    core::approx::Polynomial p(1);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::approx::Function::Parameter parameter = p.getParameter(0);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(2),1E-3));
    parameter = p.getParameter(1);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(2),1E-3));
  }
  {// 2.Order
    core::approx::FittingLinearLeastSquare fm;
    core::Curve c;
    c.append(1,11);
    c.append(2,20);
    c.append(3,33);
    core::approx::Polynomial p(2);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::approx::Function::Parameter parameter = p.getParameter(0);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(6),1E-3));
    parameter = p.getParameter(1);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(3),1E-3));
    parameter = p.getParameter(2);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(2),1E-3));
  }
  {// 3.Order
    core::approx::FittingLevenbergMarquardt fm;
    core::Curve c;
    c.append(0,42);
    c.append(1,43.1);
    c.append(2,46.4);
    c.append(3,52.5);
    core::approx::Polynomial p(3);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::approx::Function::Parameter parameter = p.getParameter(0);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(42),1E-3));
    parameter = p.getParameter(1);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(0.2),1E-3));
    parameter = p.getParameter(2);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(0.8),1E-3));
    parameter = p.getParameter(3);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(0.1),1E-3));
  }

//Approximation using LevenbergMarquardt
  {
    core::approx::FittingLevenbergMarquardt fm;
    core::Curve c;
    for_i(3){
      c.append(i,i);
    }
    core::approx::Polynomial p(1);
    QVERIFY(fm.fitWithoutCheck(p,c));//test if fitting succesfull
    //test if calculated parameters are aproximatly the same
    core::approx::Function::Parameter parameter = p.getParameter(0);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(0),1E-3));
    parameter = p.getParameter(1);
    QVERIFY(nearlyEqual(parameter.getValue(),qreal(1),1E-3));
  }
    { // 1.Order Function
      core::approx::FittingLevenbergMarquardt fm;
      core::Curve c;
      c.append(1,4);
      c.append(2,6);
      c.append(3,8);
      core::approx::Polynomial p(1);
      QVERIFY(fm.fitWithoutCheck(p,c));
      core::approx::Function::Parameter parameter = p.getParameter(0);
      QVERIFY(nearlyEqual(parameter.getValue(),qreal(2),1E-3));
      parameter = p.getParameter(1);
      QVERIFY(nearlyEqual(parameter.getValue(),qreal(2),1E-3));
    }
    {// 2.Order
      core::approx::FittingLevenbergMarquardt fm;
      core::Curve c;
      c.append(1,11);
      c.append(2,20);
      c.append(3,33);
      core::approx::Polynomial p(2);
      QVERIFY(fm.fitWithoutCheck(p,c));
      core::approx::Function::Parameter parameter = p.getParameter(0);
      QVERIFY(nearlyEqual(parameter.getValue(),qreal(6),1E-3));
      parameter = p.getParameter(1);
      QVERIFY(nearlyEqual(parameter.getValue(),qreal(3),1E-3));
      parameter = p.getParameter(2);
      QVERIFY(nearlyEqual(parameter.getValue(),qreal(2),1E-3));
    }
    {// 3.Order
      core::approx::FittingLevenbergMarquardt fm;
      core::Curve c;
      c.append(0,42);
      c.append(1,43.1);
      c.append(2,46.4);
      c.append(3,52.5);
      core::approx::Polynomial p(3);
      QVERIFY(fm.fitWithoutCheck(p,c));
      core::approx::Function::Parameter parameter = p.getParameter(0);
      QVERIFY(nearlyEqual(parameter.getValue(),42.,1E-3));
      parameter = p.getParameter(1);
      QVERIFY(nearlyEqual(parameter.getValue(),0.2,1E-3));
      parameter = p.getParameter(2);
      QVERIFY(nearlyEqual(parameter.getValue(),qreal(0.8),1E-3));
      parameter = p.getParameter(3);
      QVERIFY(nearlyEqual(parameter.getValue(),qreal(0.1),1E-3));
    }
}


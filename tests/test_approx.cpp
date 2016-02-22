#include "test_approx.h"

#include <approx/approx_functions.h>
#include <approx/approx_methods.h>


#define EQUAL_WITH_PRECISION(a,b,precision) \

//bool TestApprox::nearlyEqual(qreal a, qreal b, qreal precision) {
//  if(abs(a-b)<precision){
//      return true;
//  }
//  return false;
//}


void TestApprox::testApproxFunction() {


  {
    core::approx::FittingLinearLeastSquare fm;
    core::approx::Curve c;
    for_i(3){
      c.addPoint(core::approx::Curve::Point(i,i));
    }
    core::approx::Polynomial p(1);
    QVERIFY(fm.fitWithoutCheck(p,c));
    core::approx::Function::Parameter parameter = p.getParameter(0);

    QCOMPARE(parameter.getValue(),0.);



    p.setDegree(2);
    QVERIFY(fm.fitWithoutCheck(p,c));
    c.addPoint(core::approx::Curve::Point(4,4));
    p.setDegree(3);
    QVERIFY(fm.fitWithoutCheck(p,c));
  }

  {
    core::approx::FittingLevenbergMarquardt fm;
    core::approx::Curve c;
    for_i(3){
      c.addPoint(core::approx::Curve::Point(i,i));
    }
    core::approx::Polynomial p(1);
    QVERIFY(fm.fitWithoutCheck(p,c));
    p.setDegree(2);
    QVERIFY(fm.fitWithoutCheck(p,c));
    p.setDegree(3);
    c.addPoint(core::approx::Curve::Point(4,4));
    QVERIFY(fm.fitWithoutCheck(p,c));



  }






















}


#ifndef TEST_CORE_APPROX_H
#define TEST_CORE_APPROX_H

#include "tests_main.h"

class TestCoreApprox: public TestSuite {
  Q_OBJECT

private slots:
  void testPolynom();
  void testLinearLeastSquare();
  void testLevenbergMarquardt();
};

#endif

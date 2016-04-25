#ifndef TEST_CORE_POLEFIGURE_H
#define TEST_CORE_POLEFIGURE_H

#include "tests_main.h"

class TestCorePolefigure : public TestSuite {
  Q_OBJECT

private slots:
  void testRelatedMethods();
  void testInQuadrant();
  void testInverseDistanceWeighing();
  void testSearchInQuadrants();
  void testCalcAlphaBeta();
};

#endif

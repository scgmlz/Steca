#ifndef TEST_CORE_LENS_H
#define TEST_CORE_LENS_H

#include "tests_main.h"

class TestCoreLens: public TestSuite {
  Q_OBJECT

private slots:
  void testTransformationLens();
  void testCutLens();
  void testSensitivityCorrectionLens();
  void testIntensityRangeLens();
  void testGlobalIntensityRangeLens();
  void testNormLens();
};

#endif

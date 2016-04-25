#ifndef TEST_CORE_LENS_H
#define TEST_CORE_LENS_H

#include "tests_main.h"

#include "types/core_types_fwd.h"
#include "types/core_type_array2d.h"

class TestCoreLens: public TestSuite {
  Q_OBJECT

public:
  core::Dataset const testDataset(int width, int height, core::inten_t inten,
                                     core::inten_t specialInten,int posArray);

  core::AngleMap const testAngleMapArray(qreal valGamma, qreal gammaS,
                                              qreal valTth, qreal tthS,
                                              int width, int height,
                                              int posX, int posY);

  core::Array2D<qreal> const testCorrArray(qreal corrVal, qreal corrValSpecial,
                                                   int corrPosX, int corrPosY,
                                                   int width, int height);
private slots:
  void testTransformationLens();
  void testCutLens();
  void testSensitivityCorrectionLens();
  void testIntensityRangeLens();
  void testGlobalIntensityRangeLens();
  void testNormLens();
};


#endif // TEST_CORE_LENS_H

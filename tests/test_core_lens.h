#ifndef TEST_CORE_LENS_H
#define TEST_CORE_LENS_H

#include "core_lens.h"
#include "core_dataset.h"
#include "core_angle_map_array.h"

#include <QtTest/QtTest>

class TestCoreLens: public QObject {
  Q_OBJECT
public:
  void checkIntensity(core::shp_LensSystem const& lensSystem,
                      core::intens_t& val, core::intens_t& specialVal, int posX, int posY);

  core::Dataset const testDataset(int width, int height, core::intens_t inten,
                                     core::intens_t specialInten,int posArray);

  core::AngleMapArray const testAngleMapArray(qreal valGamma, qreal gammaS,
                                              qreal valTth, qreal tthS,
                                              int width, int height,
                                              int posX, int posY);

  core::Array2D<qreal> const testCorrArray(qreal corrVal, qreal corrValSpecial,
                                                   int corrPosX, int corrPosY,
                                                   int width, int height);
private slots:
  void testTransformationLens();
  void testROILens();
  void testSensitivityCorrectionLens();
  void testIntensityRangeLens();
  void testGlobalIntensityRangeLens();
  void testNormalizationLens();
};


#endif // TEST_CORE_LENS_H

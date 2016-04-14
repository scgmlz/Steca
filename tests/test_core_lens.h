#ifndef TEST_CORE_LENS_H
#define TEST_CORE_LENS_H

#include "core_lens.h"
#include "core_dataset.h"

#include <QtTest/QtTest>

class TestCoreLens: public QObject {
  Q_OBJECT
public:
  void checkIntensity(core::shp_Lens const& lensSystem,
                      core::inten_t& val, core::inten_t& specialVal, int posX, int posY);

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
//  void testTransformationLens();
//  void testROILens();
//  void testSensitivityCorrectionLens();
//  void testIntensityRangeLens();
//  void testGlobalIntensityRangeLens();
//  void testNormalizationLens();
};


#endif // TEST_CORE_LENS_H

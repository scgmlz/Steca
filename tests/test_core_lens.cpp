#include "test_core_lens.h"
#include "core_dataset.h"
#include "core_angle_map_array.h"
#include "core_image_transform.h"
#include "core_image_cut.h"

#define CHECK_TRANSFORM(transformNum, posX, posY) \
  imTrans = ImageTransform(transformNum);\
  lensSystem << shp_LensSystem(new TransformationLens(imTrans)); \
  checkIntensity(lensSystem,inten,specialInten,posX,posY);

#define TEST_DATA \
  int width             = 2;\
  int height            = 3;\
  intens_t inten        = 42.0f;\
  intens_t specialInten = 150.0f;\
  int posIntensArray    = 0;\
  qreal gamma           = 6.7;\
  qreal gammaSpecial    = 44.0;\
  qreal tth             = 4.2;\
  qreal tthSpecial      = 88.0;\
  int anglePosX         = 1;\
  int anglePosY         = 2;\
  Dataset dataset = testDataset(width,height,inten,specialInten,posIntensArray);\
  AngleMapArray angleMapArray = testAngleMapArray(gamma,gammaSpecial,\
                                                  tth,tthSpecial,\
                                                  width,height,\
                                                  anglePosX,anglePosY); \
  auto lensSystem = makeLensSystem(dataset, angleMapArray);

using namespace core;

void TestCoreLens::testTransformationLens() {
  TEST_DATA

  QCOMPARE(lensSystem->getPriority(), (uint) Lens::PRIORITY_PLAIN);
  checkIntensity(lensSystem,inten,specialInten,0,0);
  auto imTrans = ImageTransform(0);
  CHECK_TRANSFORM(1,2,0);
  CHECK_TRANSFORM(2,1,2);
  CHECK_TRANSFORM(3,0,1);
  CHECK_TRANSFORM(4,1,0);
  CHECK_TRANSFORM(5,2,1);
  CHECK_TRANSFORM(6,0,2);
  CHECK_TRANSFORM(7,0,0);

}

void TestCoreLens::testROILens() {
  TEST_DATA
  int cutLeft = 1;
  int cut     = 0;
  auto imageCut = ImageCut(cut,cut,cutLeft,cut);
  lensSystem << shp_LensSystem(new ROILens(imageCut));
  auto lensSize = lensSystem->getSize(); // top and bottom cut change
  QCOMPARE(lensSize, QSize(1,3));

  for_i (lensSize.height()) {
      auto angles = lensSystem->getAngles(0u,(uint) i);
      if (i == anglePosY) {
        QCOMPARE(angles.gamma,gammaSpecial);
        QCOMPARE(angles.tth, tthSpecial);
      }
  }

}

void TestCoreLens::testSensitivityCorrectionLens() {
  TEST_DATA
  qreal const corrValSpecial = 10;
  int corrVal = 1;
  int corrPosX = 0;
  int corrPosY = width-1;
  Array2D<qreal> correctionArray = testCorrArray(corrVal,corrValSpecial,
                                                 corrPosX,corrPosY,width,height);

  lensSystem << shp_LensSystem(new SensitivityCorrectionLens(correctionArray));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      auto intensity = lensSystem->getIntensity((uint) x,(uint) y);
      if (x == posIntensArray && y == posIntensArray) QCOMPARE(intensity,specialInten);
      else if (x == corrPosX && y == corrPosY) QCOMPARE(intensity,corrValSpecial*inten);
      else QCOMPARE(intensity,inten);
    }
  }

}

void TestCoreLens::testIntensityRangeLens() {
  TEST_DATA
  int cutLeft = 1;
  int cut     = 0;
  // tests for nextChangeImpl()
  lensSystem << shp_LensSystem(new IntensityRangeLens());
  auto intensityRange = lensSystem->getIntensityRange();

  QCOMPARE(intensityRange.max,specialInten);
  QCOMPARE(intensityRange.min,inten);

  auto imageCut = ImageCut(cut,cut,cutLeft,cut);
  lensSystem << shp_LensSystem(new ROILens(imageCut));
  intensityRange = lensSystem->getIntensityRange();

  QCOMPARE(intensityRange.max,inten);
  QCOMPARE(intensityRange.min,inten);

  qreal const corrValSpecial = 10;
  int corrVal = 2;
  int corrPosX = 0;
  int corrPosY = width-1;
  Array2D<qreal> correctionArray = testCorrArray(corrVal,corrValSpecial,
                                                 corrPosX,corrPosY,width,height);
  lensSystem << shp_LensSystem(new SensitivityCorrectionLens(correctionArray));

  intensityRange = lensSystem->getIntensityRange();
  QCOMPARE(intensityRange.max,inten*corrVal);
  QCOMPARE(intensityRange.min,inten*corrVal);

}

void TestCoreLens::testGlobalIntensityRangeLens() {
  TEST_DATA
  auto range = lensSystem->getIntensityRange();

  QCOMPARE(range.max, specialInten);
  QCOMPARE(range.min, inten);

  qreal globMax = 42.0;
  qreal globMin = 5.0;
  auto globRange = Range(globMin,globMax);
  lensSystem << shp_LensSystem(new GlobalIntensityRangeLens(globRange));
  range = lensSystem->getIntensityRange();

  QCOMPARE(range.max, globMax);
  QCOMPARE(range.min, globMin);
  
}

void TestCoreLens::testNormalizationLens() {
  { // basic Test
    TEST_DATA
    intens_t normVal = 2;
    lensSystem << shp_LensSystem(new NormalizationLens(normVal));
    intens_t val = normVal * inten;
    intens_t valS = normVal * specialInten;
    checkIntensity(lensSystem,val,valS,0,0);
  }
  
}


AngleMapArray const TestCoreLens::testAngleMapArray(qreal valGamma, qreal gammaS,
                                                    qreal valTth, qreal tthS,
                                                    int width, int height,
                                                    int posX, int posY) {
  QSize size(width,height);
  AngleMapArray angleMapArray;
  angleMapArray.fill(DiffractionAngles(valGamma,valTth),size);
  angleMapArray.setAt(posX,posY,DiffractionAngles(gammaS,tthS));
  return angleMapArray;
}

Array2D<qreal> const TestCoreLens::testCorrArray(qreal corrVal, qreal corrValSpecial,
                                                 int corrPosX, int corrPosY,
                                                 int width, int height) {
  Array2D<qreal> correctionArray;
  correctionArray.fill(corrVal,QSize(width,height));
  correctionArray.setAt((uint) corrPosX,(uint) corrPosY,corrValSpecial);
  return correctionArray;
}

Dataset const TestCoreLens::testDataset(int width, int height, intens_t inten,
                                        intens_t specialInten,int posArray) {
  rcstr date = "15.03.2016";
  rcstr comment = "comment";
  qreal motorXT = 1.2;
  qreal motorYT = 4.2;
  qreal motorZT = 7.3;
  qreal motorOmg = 8.9;
  qreal motorTth = 10.7;
  qreal motorPhi = 5.6;
  qreal motorChi = 1.8;
  qreal motorPST = 4.7;
  qreal motorSST = 3.5;
  qreal motorOMGM = 3.4;
  qreal mon = 5.5;
  qreal deltaTime = 55.1;
  QSize const& size = QSize(width,height);
  QVector<intens_t> intenVector;
  for_i (width * height) {
    intenVector.append(inten);
  }
  intenVector.insert(posArray,specialInten);
  intens_t const* intensities = intenVector.constData();

  return  Dataset (date,comment,
                     motorXT,motorYT,motorZT,
                     motorOmg,motorTth,motorPhi,motorChi,
                     motorPST,motorSST,motorOMGM,
                     mon, deltaTime,
                     size, intensities);

}

void TestCoreLens::checkIntensity(shp_LensSystem const& lensSystem,
                                  intens_t& val, intens_t& specialVal,
                                  int posX, int posY) {
  auto size = lensSystem->getSize();
  for (int y = 0, yHeight = size.height(); y < yHeight; ++y) {
    for (int x = 0, xWidth = size.width(); x < xWidth; ++x) {
      auto intensity = lensSystem->getIntensity(x,y);
      if (y == posY && x == posX) QCOMPARE(intensity,specialVal);
      else QCOMPARE(intensity, val);
    }
  }
}



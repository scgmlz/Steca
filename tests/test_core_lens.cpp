#include "test_core_lens.h"
#include "core_dataset.h"
#include "core_angle_map_array.h"
#include "core_image_transform.h"

using namespace core;
void TestCoreLens::testLensSystem() {
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
  QSize const& size = QSize(2,3);
  QVector<intens_t> intenVector;
  intens_t inten = 42.0f;
  intens_t specialInten = 0.0f;
  for_i (6) {
    intenVector.append(inten);
  }
  intenVector.insert(0,0);
  intens_t const* intensities = intenVector.constData();

  Dataset dataset(date,comment,
                     motorXT,motorYT,motorZT,
                     motorOmg,motorTth,motorPhi,motorChi,
                     motorPST,motorSST,motorOMGM,
                     mon, deltaTime,
                     size, intensities);

  qreal valGamma = 6.7;
  qreal valTth = 4.2;
  AngleMapArray angleMapArray;
  angleMapArray.fill(DiffractionAngles(valGamma,valTth),size);

  auto lensSystem = makeLensSystem(dataset, angleMapArray);

  QCOMPARE(lensSystem->getPriority(), (uint) LensPriority::PLAIN);

  checkIntensity(lensSystem,inten,specialInten,0,0);

  lensSystem << shp_LensSystem(new TransformationLens(ImageTransform(1)));

  checkIntensity(lensSystem,inten,specialInten,2,0);

  lensSystem << shp_LensSystem(new TransformationLens(ImageTransform(2)));

  checkIntensity(lensSystem,inten,specialInten,1,2);

  lensSystem << shp_LensSystem(new TransformationLens(ImageTransform(3)));

  // checkIntensity(lensSystem,inten,specialInten,0,1);

}

void TestCoreLens::checkIntensity(shp_LensSystem const& lensSystem,
                               intens_t& val, intens_t& specialVal, int posX, int posY) {
  auto size = lensSystem->getSize();
  for (int y = 0, yHeight = size.height(); y < yHeight; ++y) {
    for (int x = 0, xWidth = size.width(); x < xWidth; ++x) {
      auto intensity = lensSystem->getIntensity(x,y);
      if (y == posY && x == posX) QCOMPARE(intensity,specialVal);
      else QCOMPARE(intensity, val);
    }
  }

}



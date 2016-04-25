#include "test_helpers.h"
#include "types/core_type_geometry.h"

TestHelpers::TestHelpers() {
}

core::Array2D<qreal> const TestHelpers::testCorrArray(qreal corrVal, int width, int height) {
  core::Array2D<qreal> correctionArray;
  correctionArray.fill(corrVal,QSize(width,height));
  return correctionArray;
}

/// requires 10 motor angles
core::Dataset const TestHelpers::testDataset(QSize size, core::inten_t inten, QVector<qreal> motorAngles, qreal mon, qreal deltaTime) {
  rcstr date = "15.03.2016";
  rcstr comment = "comment";
  qreal motorXT = motorAngles[0];
  qreal motorYT = motorAngles[1];
  qreal motorZT = motorAngles[2];
  qreal motorOmg = motorAngles[3];
  qreal motorTth = motorAngles[4];
  qreal motorPhi = motorAngles[5];
  qreal motorChi = motorAngles[6];
  qreal motorPST = motorAngles[7];
  qreal motorSST = motorAngles[8];
  qreal motorOMGM = motorAngles[9];
  QVector<core::inten_t> intenVector;
  for_i (size.width() * size.height()) {
    intenVector.append(inten);
  }
  core::inten_t const* intensities = intenVector.constData();

  return core::Dataset (date,comment,
                     motorXT,motorYT,motorZT,
                     motorOmg,motorTth,motorPhi,motorChi,
                     motorPST,motorSST,motorOMGM,
                     mon, deltaTime,
                     size, intensities);

}

// eof

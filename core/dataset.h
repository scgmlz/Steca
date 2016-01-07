#ifndef DATASET_H
#define DATASET_H

#include "coredefs.h"
#include "image.h"
#include <QSize>
#include <QVector>

namespace core {

class QSHARED_EXPORT Dataset {
public:
  Dataset(QSize const&, Image::type const* intensities,
          rcstr comment,
          float motorXT,  float motorYT,  float motorZT, float motorOmg, float motorTth,
          float motorPhi, float motorChi,
          double mon, double deltaTime,
          float motorPST, float motorSST, float motorOMGM);
private:
  str comment;
  Image intensities;

  float
    motorXT,  motorYT,  motorZT,  motorOmg, motorTth,
    motorPhi, motorChi, motorPST, motorSST, motorOMGM;

  double mon, deltaTime;
};

typedef QVector<Dataset*> Datasets;

}

#endif

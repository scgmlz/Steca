#ifndef CORE_DATASET_H
#define CORE_DATASET_H

#include "coredefs.h"
#include "image.h"
#include <QSize>
#include <QVector>

namespace core {

class QSHARED_EXPORT Dataset {
public:
  Dataset(QSize const&, int const* intensities,
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

class QSHARED_EXPORT Datasets final: public QVector<Dataset*> {
public:
  Datasets();
 ~Datasets();
};

}

#endif

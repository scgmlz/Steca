#ifndef CORE_DATASET_H
#define CORE_DATASET_H

#include "core_defs.h"
#include "core_image.h"
#include <QSize>
#include <QVector>
#include <QMetaType>

namespace core {

class File;
class Dataset {
public:
  Dataset(File&,
          rcstr date, rcstr comment,
          qreal motorXT,  qreal motorYT,  qreal motorZT,
          qreal motorOmg, qreal motorTth, qreal motorPhi, qreal motorChi,
          qreal motorPST, qreal motorSST, qreal motorOMGM,
          qreal mon, qreal deltaTime,
          QSize const&, Image::intensity_t const* intensities);

  Image const& getImage()   const { return image;   }
  File  const& getFile()    const { return file;    }

  enum enumAttribute {
    DATE, COMMENT,
    MOTOR_X, MOTOR_Y, MOTOR_Z, MOTOR_OMG, MOTOR_TTH, MOTOR_PHI, MOTOR_CHI,
    MOTOR_PST, MOTOR_SST, MOTOR_OMGM, MON, DELTA_TIME,
    NUM_ATTRIBUTES
  };

  str getAttributeStrValue(int /*as: enumAttribute*/) const;
  static str const attributeTag[NUM_ATTRIBUTES];

private:
  File &file;
  str   date, comment;

  qreal
    motorXT,  motorYT,  motorZT,  motorOmg, motorTth,
    motorPhi, motorChi, motorPST, motorSST, motorOMGM;

  qreal mon, deltaTime;

  Image image;
};

class Datasets final: public QVector<QSharedPointer<Dataset>> {
public:
  Datasets();
};

}

// used in signals
typedef core::Dataset const *pcCoreDataset;
Q_DECLARE_METATYPE(pcCoreDataset)

#endif

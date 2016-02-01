#ifndef CORE_DATASET_H
#define CORE_DATASET_H

#include "core_defs.h"
#include "core_image.h"
#include <QSize>
#include <QMetaType>

namespace core {

class Datasets;

class Dataset {
  friend class File;
public:
  Dataset(Datasets&,
          rcstr date, rcstr comment,
          qreal motorXT,  qreal motorYT,  qreal motorZT,
          qreal motorOmg, qreal motorTth, qreal motorPhi, qreal motorChi,
          qreal motorPST, qreal motorSST, qreal motorOMGM,
          qreal mon, qreal deltaTime,
          uint size, Image::intensity_t const* intensities);

  Datasets const& getDatasets() const { return datasets;  }
  Image    const& getImage()    const { return image;     }

  str getAttributeStrValue(int /*as: enumAttribute*/) const;

  qreal tthMitte() const { return motorTth; } // TODO ? is mitte

private:
  Datasets &datasets;

  str
    date, comment;

  qreal
    motorXT,  motorYT,  motorZT,  motorOmg, motorTth,
    motorPhi, motorChi, motorPST, motorSST, motorOMGM;

  qreal
    mon, deltaTime;

  Image image;
};

class Datasets final: public QVector<QSharedPointer<Dataset>> {
public:
  Datasets();

  // declared here, because it must be the same for each individual dataset
  // might eventually be dependend on the file format, made dynamic,
  // or otherwise amended
  enum enumAttribute {
    DATE, COMMENT,
    MOTOR_X, MOTOR_Y, MOTOR_Z, MOTOR_OMG, MOTOR_TTH, MOTOR_PHI, MOTOR_CHI,
    MOTOR_PST, MOTOR_SST, MOTOR_OMGM, MON, DELTA_TIME,
    NUM_ATTRIBUTES
  };

private:
  static QVector<str> const attributeTags; // [NUM_ATTRIBUTES];

public:
  static rcstr getAttributeTag(int i) { return attributeTags.at(i); }

  uint getImageSize() const; // returns 0 if inconsistent
  Image::intensity_t getMaximumIntensity() const;
};

}

// used in signals
typedef core::Dataset const *pcCoreDataset;
Q_DECLARE_METATYPE(pcCoreDataset)

#endif

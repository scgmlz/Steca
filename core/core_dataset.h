/** \file
 * Dataset = a collection of images + metadata.
 */

#ifndef CORE_DATASET_H
#define CORE_DATASET_H

#include "core_defs.h"
#include "core_image.h"
#include <QMetaType>

namespace core {

class Dataset {
  friend class File;

private:
  static QVector<str> const attributeTags;
public:
  enum {
    DATE, COMMENT,
    MOTOR_X, MOTOR_Y, MOTOR_Z, MOTOR_OMG, MOTOR_TTH, MOTOR_PHI, MOTOR_CHI,
    MOTOR_PST, MOTOR_SST, MOTOR_OMGM, MON, DELTA_TIME,
    NUM_ATTRIBUTES
  };

  static rcstr getAttributeTag(int i);

  Dataset(rcstr date, rcstr comment,
          qreal motorXT,  qreal motorYT,  qreal motorZT,
          qreal motorOmg, qreal motorTth, qreal motorPhi, qreal motorChi,
          qreal motorPST, qreal motorSST, qreal motorOMGM,
          qreal mon, qreal deltaTime,
          uint size, Image::intens_t const* intensities);

  str getAttributeStrValue(int /*as: enumAttribute TODO */) const;
  qreal tthMitte() const { return motorTth; } // TODO ? is mitte

  Image const& getImage() const { return image; }

private:
  str
    date, comment;

  qreal
    motorXT,  motorYT,  motorZT,  motorOmg, motorTth,
    motorPhi, motorChi, motorPST, motorSST, motorOMGM;

  qreal
    mon, deltaTime;

  Image image;
};

typedef QSharedPointer<Dataset> shp_Dataset;
typedef QVector<shp_Dataset> Dataset_vec;
}


typedef core::Dataset const *pcCoreDataset; // TODO use shp_Dataset
Q_DECLARE_METATYPE(pcCoreDataset)

#endif

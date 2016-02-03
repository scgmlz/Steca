/** \file
 * Dataset = a collection of images + metadata.
 */

#ifndef CORE_DATASET_H
#define CORE_DATASET_H

#include "core_defs.h"
#include "core_image.h"
#include <QMetaType>

namespace core {

class File;

class Dataset final {
private:
  static QVector<str> const attributeTags;
public:
  enum eAttributes {
    DATE, COMMENT,
    MOTOR_X, MOTOR_Y, MOTOR_Z, MOTOR_OMG, MOTOR_TTH, MOTOR_PHI, MOTOR_CHI,
    MOTOR_PST, MOTOR_SST, MOTOR_OMGM, MON, DELTA_TIME,
    NUM_ATTRIBUTES
  };

  static rcstr getAttributeTag(int i);

public:
  Dataset(File const&, rcstr date, rcstr comment,
          qreal motorXT,  qreal motorYT,  qreal motorZT,
          qreal motorOmg, qreal motorTth, qreal motorPhi, qreal motorChi,
          qreal motorPST, qreal motorSST, qreal motorOMGM,
          qreal mon, qreal deltaTime,
          uint size, Image::intens_t const* intensities);

  str getAttributeStrValue(int /*as: enumAttribute TODO */) const;
  qreal tthMitte() const { return motorTth; } // TODO ? is mitte

  File  const& getFile()  const { return file;  }
  Image const& getImage() const { return image; }

  /// used for correction files
  void addIntensities(Dataset const&);

private:
  File const &file; ///< the parent file

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

}

Q_DECLARE_METATYPE(core::shp_Dataset)

#endif

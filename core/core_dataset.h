// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_dataset.h
//! @brief     Dataset = a collection of images + metadata.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_DATASET_H
#define CORE_DATASET_H

#include "core_image.h"

namespace core {
//------------------------------------------------------------------------------

class File;
class Reflection;
class ReflectionInfo;
class Session;

class Dataset final {
public:
  // attribute list - will be dynamic
  static uint  numAttributes();
  static rcstr getAttributeTag(uint);

public:
  Dataset(rcstr date, rcstr comment,
          qreal motorXT,  qreal motorYT,  qreal motorZT,
          qreal motorOmg, qreal motorTth, qreal motorPhi, qreal motorChi,
          qreal motorPST, qreal motorSST, qreal motorOMGM,
          qreal monCount, qreal dTime,
          QSize const& size, intens_t const* intensities);

  str getAttributeStrValue(uint) const;

  qreal middleTth()     const;
  qreal monitorCount()  const;
  qreal deltaTime()     const;

  File  const& parentFile() const;
  Image const& getImage()   const { return image; }

  Range intensRange(bool global) const;


  /// used for correction files if there are more than one image
  void addIntensities(Dataset const&);

  ReflectionInfo makeReflectionInfo(Session const& session,
                                    Reflection const& reflection,
                                    Range const& gammaSector) const;

private:
  friend class File;
  File const *file; ///< the parent file

  str
    date, comment;

  // all stored angles in degrees
  qreal
    motorXT,  motorYT,  motorZT,  motorOmg, motorTth,
    motorPhi, motorChi, motorPST, motorSST, motorOMGM;

  qreal
    monCount, dTime;

  Image image;
};

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_Dataset)

#endif // CORE_DATASET_H

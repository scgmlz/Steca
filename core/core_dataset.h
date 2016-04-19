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

#include "types/core_type_image.h"

namespace core {
//------------------------------------------------------------------------------

class Datasets;

/// Dataset = image + metadata
class Dataset final {
  friend class Datasets;
public:
  // attribute list - will be dynamic
  static uint  numAttributes();
  static rcstr attributeTag(uint);

public:
  Dataset(rcstr date, rcstr comment,
          qreal motorXT,  qreal motorYT,  qreal motorZT,
          qreal motorOmg, qreal motorTth, qreal motorPhi, qreal motorChi,
          qreal motorPST, qreal motorSST, qreal motorOMGM,
          qreal deltaMonitorCount, qreal deltaTime,
          QSize const& size, inten_t const* intens);

  Datasets const& datasets() const;

  str attributeStrValue(uint) const;

  qreal midTth()              const;
  qreal deltaMonitorCount()   const;
  qreal deltaTime()           const;

  qreal omg() const { return motorOmg_; }
  qreal phi() const { return motorPhi_; }
  qreal chi() const { return motorChi_; }

  QSize   imageSize()          const;
  inten_t inten(uint i,uint j) const;

  /// used for correction files if there is more than one image
  void addIntens(rcDataset) THROWS;

private:
  Datasets *datasets_;  // here it belongs
  str date_, comment_;

  // all stored angles in degrees
  qreal motorXT_,  motorYT_,  motorZT_,  motorOmg_, motorTth_,
        motorPhi_, motorChi_, motorPST_, motorSST_, motorOMGM_;

  qreal deltaMonitorCount_, deltaTime_;

  Image image_;
};

/// A group of Dataset(s)
class Datasets: public QVector<shp_Dataset> {
  SUPER(Datasets,QVector<shp_Dataset>)
public:
  Datasets();

  void append(shp_Dataset);

  /// collapse datasets into one (for correction files)
  Image folded() THROWS;

  // in all datasets
  QSize imageSize()       const;
//  rcRange rgeInten() const;
  qreal avgDeltaMonitorCount() const;
  qreal avgDeltaTime()    const;

private:
  void invalidateMutables();
  // computed on demand
  mutable qreal avgMonitorCount_, avgDeltaTime_;
};

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_Dataset)

#endif // CORE_DATASET_H

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
#include "types/core_type_geometry.h"
#include "types/core_type_curve.h"

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

  Dataset(rcDataset);

  rcDatasets datasets() const;

  static shp_Dataset combine(Datasets);

  str attributeStrValue(uint) const;

  qreal midTth()            const { return motorTth_;           }
  qreal deltaMonitorCount() const { return deltaMonitorCount_;  }
  qreal deltaTime()         const { return deltaTime_;          }

  qreal omg()               const { return motorOmg_; }
  qreal phi()               const { return motorPhi_; }
  qreal chi()               const { return motorChi_; }

  rcImage image()           const { return image_; }

  QSize   imageSize()          const;
  inten_t inten(uint i,uint j) const;

private:
  Datasets *datasets_;  ///< here it belongs (or could be nullptr)

  str date_, comment_;

  // all stored angles in degrees
  qreal motorXT_,  motorYT_,  motorZT_,
        motorOmg_, motorTth_, motorPhi_, motorChi_,
        motorPST_, motorSST_, motorOMGM_;

  qreal deltaMonitorCount_, deltaTime_;

  Image image_;
};

//------------------------------------------------------------------------------

/// A group of Dataset(s)
class Datasets: public QVector<shp_Dataset> {
  SUPER(Datasets,QVector<shp_Dataset>)
public:
  Datasets();

  void appendHere(shp_Dataset);

  /// collapse datasets' images into one
  Image folded() const THROWS;

  /// all dataset(s) must have the same image size
  QSize imageSize()            const;
  qreal avgDeltaMonitorCount() const;
  qreal avgDeltaTime()         const;
  rcRange rgeFixedInten(rcSession, bool trans, bool cut) const;
  rcCurve makeAvgCurve(rcSession, bool trans, bool cut) const;

private:
  void invalidateMutables();

  /// computed on demand (NaNs or emptiness indicate yet unknown values)
  mutable qreal avgMonitorCount_, avgDeltaTime_;
  mutable Range rgeFixedInten_;
  mutable Curve avgCurve_;
};

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_Dataset)

#endif // CORE_DATASET_H

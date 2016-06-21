// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_dataset.h
//! @brief     Dataset = a collection of images + metadata.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CORE_DATASET_H
#define CORE_DATASET_H

#include "types/core_type_curve.h"
#include "types/core_type_geometry.h"
#include "types/core_type_image.h"
#include "types/core_type_variant.h"

namespace core {
//------------------------------------------------------------------------------

struct Metadata {
  // attribute list - will be dynamic
  static uint    numAttributes(bool onlyNum);
  static rcstr   attributeTag(uint);
  static str_lst attributeTags();
  static cmp_vec attributeCmps();

  str      attributeStrValue(uint) const;
  QVariant attributeValue(uint)    const;
  row_t    attributeValues()       const;

  static row_t attributeNaNs();

  str date, comment;

  deg motorXT, motorYT, motorZT, motorOmg, motorTth, motorPhi, motorChi,
      motorPST, motorSST, motorOMGM;

  qreal monitorCount, deltaMonitorCount;
  qreal time, deltaTime;
};

//------------------------------------------------------------------------------
/// Dataset = metadata + image

class Dataset final {
  friend class Datasets;
public:
  Dataset(rcMetadata, size2d const&, inten_t const*);
  Dataset(rcDataset);
  Dataset();

  rcDatasets   datasets() const;
  shp_Metadata metadata() const;

  static shp_Dataset combine(Datasets);

  deg   midTth()            const { return md_->motorTth; }
  qreal deltaMonitorCount() const { return md_->deltaMonitorCount; }
  qreal deltaTime()         const { return md_->deltaTime; }

  deg omg() const { return md_->motorOmg; }
  deg phi() const { return md_->motorPhi; }
  deg chi() const { return md_->motorChi; }

  rcImage image() const { return image_; }

  size2d  imageSize() const;
  inten_t inten(uint i, uint j) const;

private:
  Datasets* datasets_;  ///< here it belongs (or could be nullptr)

  shp_Metadata md_;
  Image        image_;
};

//------------------------------------------------------------------------------

/// A group of Dataset(s)
class Datasets : public vec<shp_Dataset> {
  SUPER(Datasets, vec<shp_Dataset>)
public:
  Datasets();

  void appendHere(shp_Dataset);

  /// collapse datasets' images into one
  Image folded() const THROWS;

  /// all dataset(s) must have the same image size
  size2d  imageSize() const;
  qreal   avgDeltaMonitorCount() const;
  qreal   avgDeltaTime() const;
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

#endif  // CORE_DATASET_H

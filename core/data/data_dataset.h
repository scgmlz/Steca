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

#ifndef FWD_DATA_DATASET_H
#define FWD_DATA_DATASET_H

#include <QSharedPointer>

namespace data {

struct Metadata;
class  OneDataset;
class  OneDatasets;
class  Dataset;
class  Datasets;

typedef QSharedPointer<Metadata   const> shp_Metadata; // no changing these
typedef QSharedPointer<OneDataset const> shp_OneDataset;
typedef QSharedPointer<Dataset> shp_Dataset;

}

Q_DECLARE_METATYPE(data::shp_Dataset)

#endif

#ifndef FORWARD_DECLARATIONS
#ifndef DATA_DATASET_H
#define DATA_DATASET_H

#include "def/defs.h"
#include "typ/typ_array2d.h"
#include "typ/typ_curve.h"
#include "typ/typ_image.h"
#include "typ/typ_strlst.h"
#include "typ/typ_types.h"
#include "typ/typ_variant.h"

namespace core {
class Session;
}

namespace data {
//------------------------------------------------------------------------------

struct Metadata {
  CLS(Metadata)

  Metadata();

  // attribute list - will be dynamic
  static uint numAttributes(bool onlyNum);

  static rcstr        attributeTag(uint);
  static str_lst      attributeTags();
  static typ::cmp_vec attributeCmps();

  str        attributeStrValue(uint) const;
  QVariant   attributeValue(uint)    const;
  typ::row_t attributeValues()       const;

  static typ::row_t attributeNaNs();

  str date, comment;

  typ::deg motorXT, motorYT, motorZT, motorOmg, motorTth, motorPhi, motorChi,
           motorPST, motorSST, motorOMGM;

  qreal monitorCount, deltaMonitorCount;
  qreal time, deltaTime;
};

//------------------------------------------------------------------------------
// OneDataset = metadata + image
// for calculation always accessed through its owning Dataset

class OneDataset final {
  CLS(OneDataset) friend class OneDatasets; friend class Dataset;
public:
//  >>>OneDataset();
  OneDataset(Metadata::rc, typ::size2d::rc, not_null<inten_t const*>);
  OneDataset(rc);

//>>>  Datasets const& datasets() const;
  shp_Metadata metadata() const;

//>>>  static shp_Dataset combine(Datasets);

  tth_t midTth()            const { return md_->motorTth; }
  qreal deltaMonitorCount() const { return md_->deltaMonitorCount; }
  qreal deltaTime()         const { return md_->deltaTime; }

  typ::deg omg()            const { return md_->motorOmg; }
  typ::deg phi()            const { return md_->motorPhi; }
  typ::deg chi()            const { return md_->motorChi; }

  gma_rge  rgeGma(core::Session const&) const;
  tth_rge  rgeTth(core::Session const&) const;

  typ::Image::rc image()    const { return image_; }
  typ::size2d imageSize()   const;
//  inten_t inten(uint i, uint j) const;

private:
//>>>  Dataset* dataset_;  // here it belongs (or could be nullptr)

  shp_Metadata md_;
  typ::Image   image_;
};

//------------------------------------------------------------------------------

class OneDatasets final : public typ::vec<shp_OneDataset> {
  CLS(OneDatasets) SUPER(typ::vec<shp_OneDataset>)
public:
  typ::size2d imageSize()   const;
  typ::Image  foldedImage() const;
};

//------------------------------------------------------------------------------

// 1 or more OneDataset(s)
class Dataset final : private typ::vec<shp_OneDataset> {
  CLS(Dataset) SUPER(typ::vec<shp_OneDataset>)
  friend class Datasets;
public:
  Dataset();

  using super::count;
  using super::append;

  shp_Metadata    metadata() const;
  Datasets const& datasets() const;

  typ::deg omg() const;
  typ::deg phi() const;
  typ::deg chi() const;

  gma_rge  rgeGma(core::Session const&) const;
  tth_rge  rgeTth(core::Session const&) const;

  qreal avgDeltaMonitorCount() const;
  qreal avgDeltaTime()         const;

private:
  // all dataset(s) must have the same image size
  typ::size2d imageSize() const;

  Datasets* datasets_;      // here it belongs (or could be nullptr)
  mutable shp_Metadata md_; // on demand, just once
};

//------------------------------------------------------------------------------

class Datasets final : public typ::vec<shp_Dataset> {
  CLS(Datasets) SUPER(typ::vec<shp_Dataset>)
public:
  Datasets();

  void appendHere(shp_Dataset);

  typ::size2d imageSize() const;

  qreal avgDeltaMonitorCount() const;
  qreal avgDeltaTime() const;
  inten_rge::rc  rgeFixedInten(core::Session const&, bool trans, bool cut) const;
  typ::Curve::rc makeAvgCurve(core::Session const&, bool trans, bool cut) const;

//private:
  void invalidateMutables();

  // computed on demand (NaNs or emptiness indicate yet unknown values)
  mutable qreal avgMonitorCount_, avgDeltaTime_;
  mutable inten_rge rgeFixedInten_;
  mutable typ::Curve avgCurve_;
};

//------------------------------------------------------------------------------
}
#endif // DATA_DATASET_H
#endif // FORWARD_DECLARATIONS

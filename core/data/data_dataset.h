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
class  Dataset;
class  Datasets;

typedef QSharedPointer<Metadata> shp_Metadata;
typedef QSharedPointer<Dataset>  shp_Dataset;

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
// Dataset = metadata + image

class Dataset final {
  friend class Datasets;
  CLS(Dataset)
public:

  Dataset(Metadata::rc, typ::size2d::rc, not_null<inten_t const*>);
  Dataset(Dataset::rc);
  Dataset();

  Datasets const& datasets() const;
  shp_Metadata metadata() const;

  static shp_Dataset combine(Datasets);

  typ::deg midTth()            const { return md_->motorTth; }
  qreal    deltaMonitorCount() const { return md_->deltaMonitorCount; }
  qreal    deltaTime()         const { return md_->deltaTime; }

  typ::deg omg() const { return md_->motorOmg; }
  typ::deg phi() const { return md_->motorPhi; }
  typ::deg chi() const { return md_->motorChi; }

  typ::Image::rc image() const { return image_; }

  typ::size2d imageSize() const;
  inten_t inten(uint i, uint j) const;

private:
  Datasets* datasets_;  // here it belongs (or could be nullptr)

  shp_Metadata md_;
  typ::Image   image_;
};

//------------------------------------------------------------------------------

// A group of Dataset(s)
class Datasets : public typ::vec<shp_Dataset> {
  CLS(Datasets) SUPER(typ::vec<shp_Dataset>)
public:
  Datasets();

  void appendHere(shp_Dataset);

  // collapse datasets' images into one
  typ::Image folded() const THROWS;

  // all dataset(s) must have the same image size
  typ::size2d imageSize() const;

  qreal   avgDeltaMonitorCount() const;
  qreal   avgDeltaTime() const;
  typ::Range::rc rgeFixedInten(core::Session const&, bool trans, bool cut) const;
  typ::Curve::rc makeAvgCurve(core::Session const&, bool trans, bool cut) const;

private:
  void invalidateMutables();

  // computed on demand (NaNs or emptiness indicate yet unknown values)
  mutable qreal avgMonitorCount_, avgDeltaTime_;
  mutable typ::Range rgeFixedInten_;
  mutable typ::Curve avgCurve_;
};

//------------------------------------------------------------------------------
}
#endif // DATA_DATASET_H
#endif // FORWARD_DECLARATIONS

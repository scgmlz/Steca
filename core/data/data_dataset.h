/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

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

  static rcstr        attributeTag(uint, bool out);
  static str_lst      attributeTags(bool out);
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
  OneDataset(Metadata::rc, typ::inten_arr::rc);
  OneDataset(Metadata::rc, typ::size2d::rc, inten_vec const&);
  OneDataset(rc);

  shp_Metadata metadata() const;

  tth_t midTth()            const { return md_->motorTth;          }

  qreal monitorCount()      const { return md_->monitorCount;      }
  qreal deltaMonitorCount() const { return md_->deltaMonitorCount; }
  qreal deltaTime()         const { return md_->deltaTime;         }

  typ::deg omg()            const { return md_->motorOmg; }
  typ::deg phi()            const { return md_->motorPhi; }
  typ::deg chi()            const { return md_->motorChi; }

  gma_rge  rgeGma(core::Session const&) const;
  gma_rge  rgeGmaFull(core::Session const&) const;
  tth_rge  rgeTth(core::Session const&) const;

  inten_rge rgeInten() const;

  typ::Image::rc image()    const { return image_; }
  typ::size2d imageSize()   const;

  void collectIntens(core::Session const&, typ::Image const* intensCorr,
                     inten_vec&, uint_vec&, gma_rge::rc,
                     tth_t minTth, tth_t deltaTth) const;
private:
  shp_Metadata md_;
  typ::Image   image_;
};

//------------------------------------------------------------------------------

class OneDatasets : public typ::vec<shp_OneDataset> {
  CLS(OneDatasets) SUPER(typ::vec<shp_OneDataset>)
public:
  typ::size2d imageSize()   const;
  typ::Image  foldedImage() const;
};

//------------------------------------------------------------------------------

// 1 or more OneDataset(s)
class Dataset final : public OneDatasets {
  CLS(Dataset) SUPER(OneDatasets)
  friend class Datasets;
public:
  Dataset();

  shp_Metadata    metadata() const;
  Datasets const& datasets() const;

  typ::deg omg() const;
  typ::deg phi() const;
  typ::deg chi() const;

  gma_rge  rgeGma(core::Session const&) const;
  gma_rge  rgeGmaFull(core::Session const&) const;
  tth_rge  rgeTth(core::Session const&) const;

  inten_rge rgeInten() const;

  qreal    avgMonitorCount()      const;
  qreal    avgDeltaMonitorCount() const;
  qreal    avgDeltaTime()         const;

  inten_vec collectIntens(core::Session const&, typ::Image const* intensCorr,
                          gma_rge::rc, bool averaged) const;

private:
  // all dataset(s) must have the same image size
  typ::size2d imageSize()  const;

  Datasets*    datasets_; // here it belongs (or can be nullptr)
  shp_Metadata md_;       // on demand, compute once
};

//------------------------------------------------------------------------------

class Datasets final : public typ::vec<shp_Dataset> {
  CLS(Datasets) SUPER(typ::vec<shp_Dataset>)
public:
  Datasets();

  void appendHere(shp_Dataset);

  typ::size2d imageSize() const;

  qreal avgMonitorCount()      const;
  qreal avgDeltaMonitorCount() const;
  qreal avgDeltaTime()         const;

  inten_rge::rc  rgeGma(core::Session const&) const;
  inten_rge::rc  rgeFixedInten(core::Session const&, bool trans, bool cut) const;

  typ::Curve avgCurve(core::Session const&, bool averaged) const;

  void  invalidateAvgMutables() const;

private:
  shp_Dataset combineAll() const;
  qreal calcAvgMutable(qreal (Dataset::*avgMth)() const) const;

  // computed on demand (NaNs or emptiness indicate yet unknown values)
  mutable qreal avgMonitorCount_, avgDeltaMonitorCount_, avgDeltaTime_;
  mutable inten_rge rgeFixedInten_;
  mutable gma_rge   rgeGma_;
  mutable typ::Curve avgCurve_;
};

//------------------------------------------------------------------------------
}
#endif // DATA_DATASET_H
#endif // FORWARD_DECLARATIONS

/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
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

#ifndef CALC_LENS_H
#define CALC_LENS_H

#include "data/data_dataset.h"
#include "def/defs.h"
#include "typ/typ_array2d.h"
#include "typ/typ_curve.h"
#include "typ/typ_geometry.h"
#include "typ/typ_image.h"
#include "typ/typ_image_transform.h"
#include "typ/typ_strlst.h"
#include "typ/typ_types.h"
#include <QSharedPointer>

namespace core {
class Session;
}

namespace calc {
//------------------------------------------------------------------------------
// View the dataset through a lens (thanks, Antti!)

class LensBase {
public:
  LensBase(core::Session const&, data::Datasets::rc,
           bool trans, bool cut,
           typ::ImageTransform::rc, typ::ImageCut::rc);

  virtual ~LensBase();

  virtual typ::size2d size() const = 0;

protected:
  typ::size2d transCutSize(typ::size2d) const;

  void doTrans(uint& i, uint& j) const;
  void doCut(uint& i, uint& j)   const;

  core::Session const& session_;
  data::Datasets::rc  datasets_;
  bool                trans_, cut_;
  typ::ImageTransform imageTransform_;
  typ::ImageCut       imageCut_;
  typ::Image const*   intensCorr_;
};

//------------------------------------------------------------------------------

class ImageLens final : public LensBase {
  CLS(ImageLens) SUPER(LensBase)
public:
  ImageLens(core::Session const&, typ::Image::rc, data::Datasets::rc,
            bool trans, bool cut);

  typ::size2d size() const;

  inten_t imageInten(uint i, uint j)  const;

  inten_rge::rc rgeInten(bool fixed) const;

private:

  typ::Image const& image_;

  mutable inten_rge rgeInten_;
};

typedef QSharedPointer<ImageLens> shp_ImageLens;

//------------------------------------------------------------------------------

class DatasetLens final : public LensBase {
  CLS(DatasetLens) SUPER(LensBase)
public:
  DatasetLens(core::Session const&, data::Dataset::rc, data::Datasets::rc, eNorm,
      bool trans, bool cut, typ::ImageTransform::rc, typ::ImageCut::rc);

  typ::size2d size()    const;

  gma_rge    rgeGma()     const;
  gma_rge    rgeGmaFull() const;
  tth_rge    rgeTth()     const;
  inten_rge  rgeInten()   const;

  typ::Curve makeCurve(bool averaged)              const;
  typ::Curve makeCurve(gma_rge::rc, bool averaged) const;

  data::Dataset::rc dataset()  const { return dataset_;   }

private:
  void setNorm(eNorm);
  inten_t normFactor_;

  data::Dataset::rc dataset_;
};

typedef QSharedPointer<DatasetLens> shp_DatasetLens;

//------------------------------------------------------------------------------
}
#endif // CALC_LENS_H

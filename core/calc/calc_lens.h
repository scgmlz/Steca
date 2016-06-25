// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      calc_lens.h
//! @brief     The lens system
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

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

class ImageLens {
  CLS(ImageLens)
public:
  ImageLens(core::Session const&, typ::Image::rc, typ::Image const* corrImage,
            data::Datasets::rc, bool trans, bool cut,
            typ::ImageCut::rc, typ::ImageTransform::rc);

  typ::size2d size() const;
  inten_t inten(uint i, uint j)  const;

  typ::Range::rc rgeInten(bool fixed) const;

protected:
  void doTrans(uint& i, uint& j) const;

  void doCut(uint& i, uint& j) const {
    i += imageCut_.left; j += imageCut_.top;
  }

  void calcSensCorr();  // detector sensitivity correction

  core::Session const& session_;

  typ::Image const&   image_;
  typ::Image const*   corrImage_;

  data::Datasets::rc  datasets_;
  bool                trans_, cut_;
  typ::ImageTransform imageTransform_;
  typ::ImageCut       imageCut_;

  typ::Array2D<inten_t> intensCorr_;
  bool                hasNaNs_;
  inten_t             normFactor_;

  mutable typ::Range rgeInten_;
};

typedef QSharedPointer<ImageLens> shp_ImageLens;

//------------------------------------------------------------------------------

class Lens : public ImageLens {
  CLS(Lens) SUPER(ImageLens)
public:
  static str_lst::rc normStrLst();

  Lens(core::Session const&, data::Dataset::rc,
       typ::Image const* corr, data::Datasets::rc,
       bool trans, bool cut, eNorm norm,
       typ::ImageCut::rc, typ::ImageTransform::rc);

  typ::Angles::rc angles(uint i, uint j) const;

  gma_rge    gmaRangeAt(tth_t) const;
  typ::Curve makeCurve(gma_rge::rc, tth_rge::rc) const;
  typ::Curve makeAvgCurve() const;

  data::Dataset::rc dataset()  const { return dataset_;   }
  typ::AngleMap::rc angleMap() const { return *angleMap_; }

private:
  void setNorm(eNorm);

  data::Dataset::rc dataset_;
  typ::shp_AngleMap angleMap_;
};

typedef QSharedPointer<Lens> shp_Lens;

//------------------------------------------------------------------------------
}
#endif // CALC_LENS_H

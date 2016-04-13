// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_lens.h
//! @brief     The lens system
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_LENS_H
#define CORE_LENS_H

#include "types/core_type_geometry.h"
#include "types/core_type_range.h"
#include "types/core_type_image_transform.h"

namespace core {
//------------------------------------------------------------------------------
/// View the dataset through a lens (thanks, Antti!)

class Lens {
public:
  enum eNorm {
    normNONE,
    normDELTA_MONITOR_COUNT,
    normDELTA_TIME,
    normBACKGROUND,
  };

  static str_lst const& normStrLst();

  Lens(bool trans, bool cut, eNorm norm, rcSession,
       rcDataset, Dataset const* corr,
       AngleMap const&, ImageCut const&, ImageTransform const&);

  QSize   size()                 const;
  inten_t inten(uint i, uint j)  const;

  Angles const& angles(uint i, uint j) const;

  rcRange rgeInten()             const;
  rcRange rgeIntenGlobal()       const;

  Curve makeCurve(rcRange gamma, rcRange tth) const;

private:
  void doTrans(uint& i, uint& j) const;
  void doCut(uint& i, uint& j)   const;

  void calcSensCorr();  ///< detector sensitivity correction
  void setNorm(eNorm);

private:
  bool trans_, cut_;

  Session  const& session_;
  Dataset  const& dataset_;
  Dataset  const* corrDataset_;

  // must keep copies of these
  AngleMap angleMap_;
  ImageCut imageCut_;
  ImageTransform imageTransform_;

  Array2D<inten_t> intensCorr_; bool hasNaNs_;
  qreal normFactor_;

  mutable Range rgeInten_, rgeIntenGlobal_;
};

//------------------------------------------------------------------------------
}

#endif // CORE_LENS_H

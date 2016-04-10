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
  bool _trans, _cut;

  Session  const& _session;
  Dataset  const& _dataset;
  Dataset  const* _corrDataset;

  // must keep copies of these
  AngleMap _angleMap;
  ImageCut _imageCut;
  ImageTransform _imageTransform;

  Array2D<inten_t> _intensCorr; bool _hasNaNs;
  qreal _normFactor;

  mutable Range _rgeInten, _rgeIntenGlobal;
};

//------------------------------------------------------------------------------
}

#endif // CORE_LENS_H

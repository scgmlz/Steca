// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_reflection_info.h
//! @brief     Reflection data.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef REFLECTION_INFO_H
#define REFLECTION_INFO_H

#include "types/core_angles.h"
#include "types/core_type_range.h"
#include "types/core_type_variant.h"

namespace core {
//------------------------------------------------------------------------------

class ReflectionInfo final {
public:
  static str_lst dataTags();
  static cmp_vec dataCmps();

  ReflectionInfo();
  ReflectionInfo(shp_Metadata, deg alpha, deg beta, rcRange rgeGamma,
                 qreal inten, deg tth, qreal fwhm);
  ReflectionInfo(shp_Metadata, deg alpha, deg beta, rcRange rgeGamma);

  deg alpha() const { return alpha_; }
  deg beta() const { return beta_; }

  rcRange rgeGamma() const { return rgeGamma_; }

  qreal inten() const { return inten_; }
  deg   tth() const { return tth_; }
  qreal fwhm() const { return fwhm_; }

  // TODO these OUT?
  void setInten(qreal v) { inten_ = v; }
  void setTth(qreal v) { tth_ = v; }
  void setFwhm(qreal v) { fwhm_ = v; }

  row_t data() const;

private:
  shp_Metadata md_;
  deg          alpha_, beta_;
  Range        rgeGamma_;
  qreal        inten_;
  deg          tth_;
  qreal        fwhm_;
};

//------------------------------------------------------------------------------

class ReflectionInfos : protected QVector<ReflectionInfo> {
  SUPER(ReflectionInfos, QVector<ReflectionInfo>)
public:
  ReflectionInfos();
  using super::isEmpty;
  using super::count;
  using super::reserve;
  using super::first;
  using super::at;
  using super::begin;
  using super::end;

  void append(rcReflectionInfo);

  qreal   averageInten() const;
  rcRange rgeInten() const;

private:
  void          invalidate();
  mutable qreal avgInten_;
  mutable Range rgeInten_;
};

//------------------------------------------------------------------------------
}
#endif

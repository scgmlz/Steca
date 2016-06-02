// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_reflection_info.h
//! @brief     Reflection data.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
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
  enum class eField {
    ALPHA, BETA, GAMMA1, GAMMA2,
    INTEN, INTEN_ERROR,
    TTH,   INTEN_TTH,
    FWHM,  INTEN_FWHM,
  };

  enum class eReflAttr {
    ALPHA, BETA, GAMMA1, GAMMA2,
    INTEN, SIGMA_INTEN, TTH, SIGMA_TTH, FWHM, SIGMA_FWHM,
    NUM_REFL_ATTR,
  };

  static str_lst dataTags();
  static cmp_vec dataCmps();

  static str const reflStringTag(uint attr);

  ReflectionInfo();

  ReflectionInfo(shp_Metadata, deg alpha, deg beta, rcRange rgeGamma,
                 qreal inten, qreal, deg tth, deg, qreal fwhm, qreal);
  ReflectionInfo(shp_Metadata, deg alpha, deg beta, rcRange rgeGamma);

  ReflectionInfo(deg alpha, deg beta, rcRange rgeGamma,
                 qreal inten, qreal, deg tth, deg, qreal fwhm, qreal);
  ReflectionInfo(deg alpha, deg beta);

  deg alpha() const { return alpha_; }
  deg beta()  const { return beta_;  }

  rcRange rgeGamma() const { return rgeGamma_; }

  qreal inten()      const { return inten_;      }
  qreal intenError() const { return intenError_; }

  deg   tth()        const { return tth_;        }
  deg   tthError()   const { return tthError_;   }

  qreal fwhm()       const { return fwhm_;  }
  qreal fwhmError()  const { return fwhmError_;  }

  row_t data() const;

private:
  shp_Metadata md_;
  deg          alpha_, beta_;
  Range        rgeGamma_;
  qreal        inten_, intenError_;
  deg          tth_,   tthError_;
  qreal        fwhm_,  fwhmError_;
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

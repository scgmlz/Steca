// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      calc_reflection_info.h
//! @brief     Reflection data.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CALC_REFLECTION_INFO_H
#define CALC_REFLECTION_INFO_H

#include "def/defs.h"
#include "data/data_dataset.h"
#include "typ/typ_angles.h"
#include "typ/typ_range.h"
#include "typ/typ_strlst.h"
#include "typ/typ_types.h"
#include "typ/typ_variant.h"

namespace calc {
//------------------------------------------------------------------------------

class ReflectionInfo final {
  THIS(ReflectionInfo)
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

  static str_lst      dataTags();
  static typ::cmp_vec dataCmps();

  static str const reflStringTag(uint attr);

  ReflectionInfo();

  ReflectionInfo(data::shp_Metadata, typ::deg alpha, typ::deg beta,
                 typ::Range::rc rgeGamma,
                 typ::inten_t, qreal, typ::deg tth, typ::deg, qreal fwhm, qreal);
  ReflectionInfo(data::shp_Metadata, typ::deg alpha, typ::deg beta,
                 typ::Range::rc rgeGamma);

  ReflectionInfo(typ::deg alpha, typ::deg beta, typ::Range::rc rgeGamma,
                 typ::inten_t, qreal, typ::deg tth, typ::deg, qreal fwhm, qreal);
  ReflectionInfo(typ::deg alpha, typ::deg beta);

  typ::deg   alpha() const { return alpha_; }
  typ::deg   beta()  const { return beta_;  }

  typ::Range::rc rgeGamma() const { return rgeGamma_; }

  typ::inten_t inten()  const { return inten_;      }
  qreal intenError()    const { return intenError_; }

  typ::deg   tth()      const { return tth_;        }
  typ::deg   tthError() const { return tthError_;   }

  qreal fwhm()          const { return fwhm_;  }
  qreal fwhmError()     const { return fwhmError_;  }

  typ::row_t data() const;

private:
  data::shp_Metadata md_;
  typ::deg           alpha_, beta_;
  typ::Range         rgeGamma_;
  typ::inten_t       inten_;
  qreal              intenError_;
  typ::deg           tth_, tthError_;
  qreal              fwhm_, fwhmError_;
};

//------------------------------------------------------------------------------

class ReflectionInfos : public typ::vec<ReflectionInfo> {
  SUPER(ReflectionInfos, typ::vec<ReflectionInfo>)
public:
  ReflectionInfos();

  void append(ReflectionInfo::rc);

  qreal   averageInten() const;
  typ::Range::rc rgeInten() const;

private:
  void          invalidate();
  mutable qreal avgInten_;
  mutable typ::Range rgeInten_;
};

//------------------------------------------------------------------------------
}
#endif // CALC_REFLECTION_INFO_H

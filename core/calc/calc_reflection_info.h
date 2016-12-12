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

#ifndef CALC_REFLECTION_INFO_H
#define CALC_REFLECTION_INFO_H

#include "def/defs.h"
#include "data/data_dataset.h"
#include "typ/typ_strlst.h"
#include "typ/typ_types.h"
#include "typ/typ_variant.h"

namespace calc {
//------------------------------------------------------------------------------

class ReflectionInfo final {
  CLS(ReflectionInfo)
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

  static str_lst      dataTags(bool out);
  static typ::cmp_vec dataCmps();

  static str const reflStringTag(uint attr, bool out);

  ReflectionInfo();

  ReflectionInfo(data::shp_Metadata, typ::deg alpha, typ::deg beta, gma_rge,
                 inten_t, inten_t /*error*/, tth_t, tth_t /*error*/, fwhm_t, fwhm_t /*error*/);
  ReflectionInfo(data::shp_Metadata, typ::deg alpha, typ::deg beta, gma_rge);
  ReflectionInfo(typ::deg alpha, typ::deg beta, gma_rge,
                 inten_t, inten_t /*error*/, tth_t, tth_t /*error*/, fwhm_t, fwhm_t /*error*/);
  ReflectionInfo(typ::deg alpha, typ::deg beta);

  typ::deg alpha()      const { return alpha_;  }
  typ::deg beta()       const { return beta_;   }

  gma_rge  rgeGma()     const { return rgeGma_; }

  inten_t  inten()      const { return inten_;      }
  inten_t  intenError() const { return intenError_; }

  tth_t    tth()        const { return tth_;        }
  tth_t    tthError()   const { return tthError_;   }

  fwhm_t   fwhm()       const { return fwhm_;       }
  fwhm_t   fwhmError()  const { return fwhmError_;  }

  typ::row_t data() const;

private:
  data::shp_Metadata md_;

  typ::deg alpha_, beta_;
  gma_rge  rgeGma_;

  inten_t inten_, intenError_;
  tth_t   tth_,   tthError_;
  fwhm_t  fwhm_,  fwhmError_;
};

//------------------------------------------------------------------------------

class ReflectionInfos : public typ::vec<ReflectionInfo> {
  CLS(ReflectionInfos) SUPER(typ::vec<ReflectionInfo>)
public:
  ReflectionInfos();

  void append(ReflectionInfo::rc);

  inten_t       averageInten() const;
  inten_rge::rc rgeInten()     const;

private:
  void invalidate();

  mutable inten_t   avgInten_;
  mutable inten_rge rgeInten_;
};

//------------------------------------------------------------------------------
}
#endif // CALC_REFLECTION_INFO_H

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

#ifndef CALC_REFLECTION_H
#define CALC_REFLECTION_H

#include "typ/typ_types.h"
#include "fit/fit_fun.h"
#include "typ/typ_strlst.h"
#include <QSharedPointer>

namespace calc {
//------------------------------------------------------------------------------

class Reflection final {
  CLS(Reflection)
public:

  static str_lst::rc typeStrLst();
  static rcstr       typeTag(fit::ePeakType);

  Reflection(fit::ePeakType = fit::ePeakType::RAW);

  fit::ePeakType type() const;
  void setType(fit::ePeakType);

  fit::PeakFunction::rc peakFunction() const;  // REMOVE

  typ::Range::rc range() const;
  void setRange(typ::Range::rc);

  void invalidateGuesses();

  void setGuessPeak(peak_t::rc peak) { peakFunction_->setGuessedPeak(peak); }
  void setGuessFWHM(fwhm_t fwhm)     { peakFunction_->setGuessedFWHM(fwhm); }

  void fit(typ::Curve::rc);

private:
  void setPeakFunction(fit::ePeakType);
  void setPeakFunction(fit::PeakFunction*);

  scoped<fit::PeakFunction*> peakFunction_;

public:
  typ::JsonObj saveJson() const;
  void loadJson(typ::JsonObj::rc) THROWS;
};

typedef QSharedPointer<Reflection>  shp_Reflection;
typedef typ::vec<shp_Reflection> Reflections;

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(calc::shp_Reflection)

#endif // CORE_REFLECTION_H

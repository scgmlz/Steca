/** \file
 * Detector images.
 */

#ifndef CORE_REFLECTION_H
#define CORE_REFLECTION_H

#include "types/core_types_fwd.h"
#include "core_fit_functions.h"

namespace core {
//------------------------------------------------------------------------------

class Reflection {
public:
  using eType = fit::PeakFunction::eType;

  static str_lst const& reflTypes();

  Reflection(eType = eType::PEAK_GAUSSIAN);
 ~Reflection();

  eType getType() const;
  void  setType(eType);

  Range const& getRange() const { return range; }
  void         setRange(Range const&);

  fit::PeakFunction* makePeakFunction() const;

  fit::PeakFunction& getPeakFunction();
  fit::PeakFunction const& getPeakFunction() const;

  void invalidateGuesses();

  void setGuessPeak(XY const& xy) { peakFunction->setGuessPeak(xy);   }
  void setGuessFWHM(qreal fwhm)   { peakFunction->setGuessFWHM(fwhm); }

private:
  Range range;

  void setPeakFunction(eType);
  void setPeakFunction(fit::PeakFunction*);
  fit::PeakFunction *peakFunction;

public:
  void    loadJson(rcJsonObj) THROWS;
  JsonObj saveJson() const;
};

typedef QVector<shp_Reflection>    Reflections;

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_Reflection)

#endif

/** \file
 * Detector images.
 */

#ifndef CORE_REFLECTION_H
#define CORE_REFLECTION_H

#include "core_types.h"
#include "core_array2d.h"
#include "core_fit_functions.h"

namespace core {
//------------------------------------------------------------------------------

class Reflection {
public:
  using PeakFunction = core::fit::PeakFunction;
  using eType        = PeakFunction::eType;

  static str_lst const& reflTypes();

  Reflection(eType);
 ~Reflection();

  eType getType() const;
  void  setType(eType);

  Range const& getRange() const { return range; }
  void         setRange(Range const&);

  PeakFunction& getPeakFunction() const;

  void invalidateGuesses();
  void setGuessPeak(XY const& xy) { peakFunction->setGuessPeak(xy);   }
  void setGuessFWHM(qreal fwhm)   { peakFunction->setGuessFWHM(fwhm); }

private:
  Range range;

  void setPeakFunction(eType);
  PeakFunction *peakFunction;
};

typedef QSharedPointer<Reflection> shp_Reflection;
typedef QVector<shp_Reflection>    Reflections;

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_Reflection)

#endif

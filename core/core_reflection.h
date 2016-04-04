// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_reflection.h
//! @brief     Reflections
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_REFLECTION_H
#define CORE_REFLECTION_H

#include "types/core_types_fwd.h"
#include "core_fit_functions.h"

namespace core {
//------------------------------------------------------------------------------

class Reflection {
public:
  static str_lst const& reflTypes();
  static rcstr          reflType(ePeakType);

  Reflection(ePeakType = ePeakType::GAUSSIAN);
 ~Reflection();

  ePeakType getType() const;
  void  setType(ePeakType);

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

  void setPeakFunction(ePeakType);
  void setPeakFunction(fit::PeakFunction*);

  QScopedPointer<fit::PeakFunction> peakFunction;

public:
  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

typedef QVector<shp_Reflection>    Reflections;

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_Reflection)

#endif // CORE_REFLECTION_H

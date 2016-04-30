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
#include <QStringList>

namespace core {
//------------------------------------------------------------------------------

class Reflection final {
public:
  static str_lst const& typeStrLst();
  static rcstr          typeTag(ePeakType);

  Reflection(ePeakType = ePeakType::GAUSSIAN);

  ePeakType type() const;
  void setType(ePeakType);

  rcRange range() const { return range_; }
  void setRange(rcRange);

  fit::PeakFunction const& peakFunction() const {
    return const_cast<Reflection*>(this)->peakFunction();
  }

  fit::PeakFunction& peakFunction();

  void invalidateGuesses();

  void setGuessPeak(rcXY  xy)   { peakFunction_->setGuessedPeak(xy);   }
  void setGuessFWHM(qreal fwhm) { peakFunction_->setGuessedFWHM(fwhm); }

private:
  Range range_;

  void setPeakFunction(ePeakType);
  void setPeakFunction(fit::PeakFunction*);

  QScopedPointer<fit::PeakFunction> peakFunction_;

public:
  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_Reflection)

#endif // CORE_REFLECTION_H

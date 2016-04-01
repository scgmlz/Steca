// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_xy.h
//! @brief     2D coordinate point
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPE_XY_H
#define CORE_TYPE_XY_H

#include "core_defs.h"
#include "core_types_fwd.h"

namespace core {
//------------------------------------------------------------------------------
/// 2D point

struct XY {
  XY();                   ///< invalid (NaN)
  XY(qreal,qreal);

  qreal x, y;

  void  invalidate();     ///< make invalid
  bool  isValid() const;  ///< is not NaN

  void    loadJson(rcJsonObj) THROWS;
  JsonObj saveJson() const;
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_XY_H

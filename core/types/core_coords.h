// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      core_coords.h
//! @brief     Coordinates
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_COORDS_H
#define CORE_COORDS_H

#include "core_defs.h"
#include "core_types_fwd.h"

namespace core {
//------------------------------------------------------------------------------
/// 2D point, integers

struct IJ {
  int i, j;

  IJ();                  ///< (0,0)
  IJ(int, int);

  bool operator ==(IJ const&) const;

  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

typedef IJ const& rcIJ;

//------------------------------------------------------------------------------
/// 2D point, reals

struct XY {
  qreal x, y;

  XY();                   ///< invalid (NaN)
  XY(qreal,qreal);

  void  invalidate();     ///< make invalid
  bool  isValid() const;  ///< is not NaN

  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

typedef XY const& rcXY;

//------------------------------------------------------------------------------
}
#endif // CORE_COORDS_H

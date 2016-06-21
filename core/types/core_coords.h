// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_coords.h
//! @brief     Coordinates
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
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

  IJ();  ///< (0,0)
  IJ(int, int);

  int compare(IJ const&) const;

  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

typedef IJ const& rcIJ;

//------------------------------------------------------------------------------
/// 2D point, reals

struct XY {
  qreal x, y;

  XY();  ///< invalid (NaN)
  XY(qreal, qreal);

  void invalidate();     ///< make invalid
  bool isValid() const;  ///< is not NaN

  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

typedef XY const& rcXY;

//------------------------------------------------------------------------------
}
#endif  // CORE_COORDS_H

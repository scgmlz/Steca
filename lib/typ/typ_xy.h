// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_xy.h
//! @brief     Real coordinates
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_XY_H
#define TYP_XY_H

#include "def/def_macros.h"

namespace typ {
//------------------------------------------------------------------------------
// 2D point, reals

class JsonObj;

struct XY {
  CLS(XY)

  qreal x, y;

  XY();  // invalid (NaN)
  XY(qreal, qreal);

  int  compare(rc) const;

  void invalidate();     // make invalid (NaN)
  bool isValid() const;  // is not NaN

  JsonObj saveJson() const;
  void loadJson(JsonObj const&) THROWS;
};

//------------------------------------------------------------------------------
}
#endif // CORE_COORDS_H

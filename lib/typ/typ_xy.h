/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
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

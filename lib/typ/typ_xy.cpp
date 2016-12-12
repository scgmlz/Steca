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

#include "typ_xy.h"
#include "../def/def_compare.h"
#include "typ_json.h"

namespace typ {
//------------------------------------------------------------------------------

XY::XY() {
  invalidate();
}

XY::XY(qreal x_, qreal y_) : x(x_), y(y_) {}

int XY::compare(rc that) const {
  COMPARE_VALUE(x)
  COMPARE_VALUE(y)
  return 0;
}

void XY::invalidate() {
  x = y = qQNaN();
}

bool XY::isValid() const {
  return !qIsNaN(x) && !qIsNaN(y);
}

JsonObj XY::saveJson() const {
  return JsonObj().saveQreal(json_key::X, x).saveQreal(json_key::Y, y);
}

void XY::loadJson(JsonObj::rc obj) THROWS {
  x = obj.loadQreal(json_key::X);
  y = obj.loadQreal(json_key::Y);
}

//------------------------------------------------------------------------------
}
// eof

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

#include "typ/typ_xy.h"
#include "def/def_compare.h"
#include "typ/typ_json.h"
#include "test/tests.h"

namespace typ {
//------------------------------------------------------------------------------

XY::XY() {
  invalidate();
}

TEST("XY()", ({
  XY xy;
  CHECK(qIsNaN(xy.x));
  CHECK(qIsNaN(xy.y));
});)

XY::XY(qreal x_, qreal y_) : x(x_), y(y_) {}

TEST("XY(x,y)", ({
  XY xy(2.3,3.4);
  CHECK_EQ(2.3, xy.x);
  CHECK_EQ(3.4, xy.y);
});)

int XY::compare(rc that) const {
  EXPECT(isValid() && that.isValid())
  COMPARE_VALUE(x)
  COMPARE_VALUE(y)
  return 0;
}

TEST("XY::compare", ({
  XY xy(1,2), xy1(1,2), xy2(1,0), xy3(2,2);
  CHECK_EQ( 0, xy.compare(xy));
  CHECK_EQ( 0, xy.compare(xy1));
  CHECK_EQ( 1, xy.compare(xy2));
  CHECK_EQ(-1, xy.compare(xy3));
});)

void XY::invalidate() {
  x = y = qQNaN();
}

bool XY::isValid() const {
  return !qIsNaN(x) && !qIsNaN(y);
}

TEST("XY::valid", ({
  XY xy;
  CHECK(!xy.isValid());
  xy.x = 0;
  CHECK(!xy.isValid());
  xy.y = 0;
  CHECK(xy.isValid());
  xy.invalidate();
  CHECK(!xy.isValid());
});)

JsonObj XY::saveJson() const {
  return JsonObj().saveQreal(json_key::X, x).saveQreal(json_key::Y, y);
}

void XY::loadJson(JsonObj::rc obj) THROWS {
  x = obj.loadQreal(json_key::X);
  y = obj.loadQreal(json_key::Y);
}

TEST("XY::json", ({
  XY xy(-1,2), xy1;
  xy1.loadJson(xy.saveJson());
  CHECK_EQ(0, xy.compare(xy1));
});)

//------------------------------------------------------------------------------
}
// eof

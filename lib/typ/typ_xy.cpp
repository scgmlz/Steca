// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_xy.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "typ_xy.h"
#include "def/def_compare.h"
#include "typ/typ_json.h"

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

static str const KEY_X("x"), KEY_Y("y");

JsonObj XY::saveJson() const {
  return JsonObj().saveQreal(KEY_X, x).saveQreal(KEY_Y, y);
}

void XY::loadJson(JsonObj::rc obj) THROWS {
  x = obj.loadQreal(KEY_X);
  y = obj.loadQreal(KEY_Y);
}

//------------------------------------------------------------------------------
}
// eof

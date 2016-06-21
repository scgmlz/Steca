// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_coords.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "core_coords.h"
#include "types/core_json.h"

namespace core {
//------------------------------------------------------------------------------

static str const KEY_X("x"), KEY_Y("y");

IJ::IJ() : IJ(0, 0) {}

IJ::IJ(int i_, int j_) : i(i_), j(j_) {}

int IJ::compare(IJ const& that) const {
  if (i < that.i) return -1;
  if (i > that.i) return +1;
  if (j < that.j) return -1;
  if (j > that.j) return +1;
  return 0;
}

JsonObj IJ::saveJson() const {
  return JsonObj()
      .saveInt(KEY_X, i)  // yes, use KEY_X/Y
      .saveInt(KEY_Y, j);
}

void IJ::loadJson(rcJsonObj obj) THROWS {
  i = obj.loadInt(KEY_X);  // yes, use KEY_X/Y
  j = obj.loadInt(KEY_Y);
}

//------------------------------------------------------------------------------

XY::XY() {
  invalidate();
}

XY::XY(qreal x_, qreal y_) : x(x_), y(y_) {}

void XY::invalidate() {
  x = y = qQNaN();
}

bool XY::isValid() const {
  return !qIsNaN(x) && !qIsNaN(y);
}

JsonObj XY::saveJson() const {
  return JsonObj().saveReal(KEY_X, x).saveReal(KEY_Y, y);
}

void XY::loadJson(rcJsonObj obj) THROWS {
  x = obj.loadReal(KEY_X);
  y = obj.loadReal(KEY_Y);
}

//------------------------------------------------------------------------------
}
// eof

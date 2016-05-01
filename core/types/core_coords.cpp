// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_coords.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_coords.h"
#include "types/core_json.h"

namespace core {
//------------------------------------------------------------------------------

static str const KEY_X("x"), KEY_Y("y");

IJ::IJ(): IJ(0,0) {
}

IJ::IJ(int i_, int j_): i(i_), j(j_) {
}

bool IJ::operator ==(IJ const& that) const {
  return i == that.i && j == that.j;
}

JsonObj IJ::saveJson() const {
  return JsonObj()
    .saveInt(KEY_X,i) // yes, use KEY_X/Y
    .saveInt(KEY_Y,j);
}

void IJ::loadJson(rcJsonObj obj) THROWS {
  i = obj.loadInt(KEY_X); // yes, use KEY_X/Y
  j = obj.loadInt(KEY_Y);
}

//------------------------------------------------------------------------------

XY::XY() {
  invalidate();
}

XY::XY(qreal x_, qreal y_): x(x_), y(y_) {
}

void XY::invalidate() {
  x = y = qQNaN();
}

bool XY::isValid() const {
  return !qIsNaN(x) && !qIsNaN(y);
}

JsonObj XY::saveJson() const {
  return JsonObj()
    .saveReal(KEY_X,x)
    .saveReal(KEY_Y,y);
}

void XY::loadJson(rcJsonObj obj) THROWS {
  x = obj.loadReal(KEY_X);
  y = obj.loadReal(KEY_Y);
}

//------------------------------------------------------------------------------
}
// eof

// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_xy.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_type_xy.h"
#include "types/core_type_range.h"
#include "types/core_json.h"

namespace core {
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

static str const KEY_X("x"), KEY_Y("y");

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

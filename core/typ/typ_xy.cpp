// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_xy.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test002_xy
//
// ************************************************************************** //

#include "typ_xy.h"
#include "def/def_cmp_impl.h"
#include "typ/typ_json.h"

namespace typ {

XY::XY() {
    invalidate();
}

XY::XY(qreal x_, qreal y_) : x(x_), y(y_) {}

int XY::compare(rc that) const {
    EXPECT(isValid() && that.isValid())
    RET_COMPARE_VALUE(x)
    RET_COMPARE_VALUE(y)
    return 0;
}

VALID_EQ_NE_OPERATOR(XY)

void XY::invalidate() {
    x = y = NAN;
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

} // namespace typ

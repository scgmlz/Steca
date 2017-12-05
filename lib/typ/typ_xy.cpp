// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/typ/typ_xy.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ_xy.h"

#include "def/def_cmp_impl.h"
#include "typ/typ_json.h"

#include "test/tests.h"

namespace typ {

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
         XY xy(2.3, 3.4);
         CHECK_EQ(2.3, xy.x);
         CHECK_EQ(3.4, xy.y);
     });)

int XY::compare(rc that) const {
    EXPECT(isValid() && that.isValid())
    RET_COMPARE_VALUE(x)
    RET_COMPARE_VALUE(y)
    return 0;
}

VALID_EQ_NE_OPERATOR(XY)

TEST("XY::compare", ({
         XY xy(1, 2), xy1(1, 2), xy2(1, 0), xy3(2, 2);
         CHECK_EQ(0, xy.compare(xy));
         CHECK_EQ(0, xy.compare(xy1));
         CHECK_EQ(1, xy.compare(xy2));
         CHECK_EQ(-1, xy.compare(xy3));

         CHECK_EQ(xy, xy1);
         CHECK_NE(xy, xy2);
     });)

void XY::invalidate() {
    x = y = NAN;
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
         XY xy(-1, 2), xy1;
         xy1.loadJson(xy.saveJson());
         CHECK_EQ(xy, xy1);
     });)


}

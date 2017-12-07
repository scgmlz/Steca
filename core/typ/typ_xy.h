// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_xy.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_XY_H
#define TYP_XY_H

#include "def/def_cmp.h"
#include "def/def_macros.h"

namespace typ {

// 2D point, reals

class JsonObj;

struct XY {
    CLASS(XY)

    qreal x, y;

    XY() { invalidate(); }
    XY(qreal x_, qreal y_) : x(x_), y(y_) {}

    COMPARABLE

    void invalidate(); // x,y <- NAN
    bool isValid() const { return !qIsNaN(x) && !qIsNaN(y); }

    JsonObj saveJson() const;
    void loadJson(JsonObj const&) THROWS;
};

}

#endif // TYP_XY_H

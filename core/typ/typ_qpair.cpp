// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_qpair.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test002_qpair
//
// ************************************************************************** //

#include "typ_qpair.h"
#include "def/def_cmp_impl.h"
#include "typ/typ_json.h"

int qpair::compare(rc that) const {
    EXPECT(isValid() && that.isValid())
    RET_COMPARE_VALUE(x)
    RET_COMPARE_VALUE(y)
    return 0;
}

VALID_EQ_NE_OPERATOR(qpair)

void qpair::invalidate() {
    x = y = NAN;
}

using typ::JsonObj;

JsonObj qpair::saveJson() const {
    return JsonObj().saveQreal(json_key::X, x).saveQreal(json_key::Y, y);
}

void qpair::loadJson(JsonObj::rc obj) THROWS {
    x = obj.loadQreal(json_key::X);
    y = obj.loadQreal(json_key::Y);
}

// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/realpair.cpp
//! @brief     Implements class qpair
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test002_qpair
//
// ************************************************************************** //

#include "def/comparators.h"
#include "typ/exception.h"
#include "typ/json.h"
#include "typ/realpair.h"

int qpair::compare(qpair const& that) const {
    debug::ensure(isValid() && that.isValid());
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

void qpair::loadJson(JsonObj const& obj) THROWS {
    x = obj.loadQreal(json_key::X);
    y = obj.loadQreal(json_key::Y);
}

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

QJsonObject qpair::to_json() const {
    return { { "x", qreal_to_json(x) }, { "y", qreal_to_json(y) } };
}

void qpair::from_json(JsonObj const& obj) THROWS {
    x = obj.loadQreal("x");
    y = obj.loadQreal("y");
}

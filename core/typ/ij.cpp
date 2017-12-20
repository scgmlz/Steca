// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/ij.cpp
//! @brief     Implements struct IJ
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test005_ij.
//
// ************************************************************************** //

#include "def/comparators.h"
#include "typ/exception.h"
#include "typ/ij.h"
#include "typ/json.h"

namespace typ {

IJ::IJ() : IJ(0, 0) {}

IJ::IJ(int i_, int j_) : i(i_), j(j_) {}

int IJ::compare(IJ const& that) const {
    RET_COMPARE_VALUE(i)
    RET_COMPARE_VALUE(j)
    return 0;
}

EQ_NE_OPERATOR(IJ)

QJsonObject IJ::to_json() const {
    return { { "i", i }, { "j", j } };
}

void IJ::from_json(JsonObj const& obj) THROWS {
    i = obj.loadInt("i");
    j = obj.loadInt("j");
}

} // namespace typ

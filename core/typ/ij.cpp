// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/ij.cpp
//! @brief     Implements struct IJ.
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
#include "def/exception.h"
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

JsonObj IJ::saveJson() const {
    return JsonObj().saveInt(json_key::I, i).saveInt(json_key::J, j);
}

void IJ::loadJson(JsonObj const& obj) THROWS {
    i = obj.loadInt(json_key::I);
    j = obj.loadInt(json_key::J);
}

} // namespace typ

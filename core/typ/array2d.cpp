// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/array2d.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test004_array2d
//
// ************************************************************************** //

#include "core/def/comparators.h"
#include "core/typ/array2d.h"

int size2d::compare(size2d const& that) const {
    RET_COMPARE_VALUE(w)
    RET_COMPARE_VALUE(h)
    return 0;
}

EQ_NE_OPERATOR(size2d)

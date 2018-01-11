// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/def/debug.cpp
//! @brief     Implements functions for debugging
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "debug.h"

void debug::ensure(bool cond, const char* text) {
    if (!cond) qFatal(text);
}

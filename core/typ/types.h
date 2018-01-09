// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/types.h
//! @brief     Defines types inten_t, fwhm_t, inten_vec, and enum class eNorm
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYPES_H
#define TYPES_H

#include "core/typ/vec.h"

typedef float inten_t;
typedef float fwhm_t;

typedef vec<inten_t> inten_vec;

enum class eNorm {
    NONE,
    MONITOR,
    DELTA_MONITOR,
    DELTA_TIME,
    BACKGROUND,
};

#endif // TYPES_H

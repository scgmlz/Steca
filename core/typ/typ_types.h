// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_types.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_TYPES_H
#define TYP_TYPES_H

#include "typ/typ_angles.h"
#include "typ/typ_range.h"
#include "typ/typ_strlst.h"
#include "typ/typ_xy.h"

typedef float inten_t;
typedef float fwhm_t;

typedef typ::vec<inten_t> inten_vec;

typedef typ::Range inten_rge;
typedef typ::Range tth_rge;
typedef typ::Range gma_rge;

typedef typ::XY peak_t;

enum class eNorm {
    NONE,
    MONITOR,
    DELTA_MONITOR,
    DELTA_TIME,
    BACKGROUND,
};

str_lst::rc normStrLst();

#endif // TYP_TYPES_H

// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/types.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYPES_H
#define TYPES_H

#include "typ/angles.h"
#include "typ/range.h"
#include "typ/realpair.h"
#include "typ/strlst.h"

typedef float inten_t;
typedef float fwhm_t;

typedef typ::vec<inten_t> inten_vec;

typedef typ::Range inten_rge;
typedef typ::Range tth_rge;
typedef typ::Range gma_rge;

enum class eNorm {
    NONE,
    MONITOR,
    DELTA_MONITOR,
    DELTA_TIME,
    BACKGROUND,
};

str_lst const& normStrLst();

#endif // TYPES_H

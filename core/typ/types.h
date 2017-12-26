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

#include "typ/vec.h"

typedef float inten_t;
typedef float fwhm_t;

typedef typ::vec<inten_t> inten_vec;

enum class eNorm {
    NONE,
    MONITOR,
    DELTA_MONITOR,
    DELTA_TIME,
    BACKGROUND,
};

QStringList const& normStrLst();

#endif // TYPES_H

// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_types.h
//! @brief     Basic steca data types
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_TYPES_H
#define TYP_TYPES_H

#include "typ/typ_angles.h"
#include "typ/typ_range.h"
#include "typ/typ_strlst.h"
#include "typ/typ_vec.h"
#include "typ/typ_xy.h"

typedef typ::deg gma_t;
typedef typ::deg tth_t;

typedef float inten_t;
typedef float fwhm_t;

typedef typ::vec<inten_t> inten_vec;

typedef typ::Range inten_rge;
typedef typ::Range tth_rge;
typedef typ::Range gma_rge;

typedef typ::XY peak_t;

enum class eNorm {
  NONE,
  MONITOR, DELTA_MONITOR, DELTA_TIME, BACKGROUND,
};

str_lst::rc normStrLst();

//------------------------------------------------------------------------------
#endif // TYP_TYPES_H

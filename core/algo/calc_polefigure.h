// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/algo/calc_polefigure.h
//! @brief     Defines function interpolateInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CALC_POLEFIGURE_H
#define CALC_POLEFIGURE_H

#include "core/data/peak_info.h"
#include "core/typ/async.h"

PeakInfos interpolateInfos(const PeakInfos&, Progress*);

#endif // CALC_POLEFIGURE_H

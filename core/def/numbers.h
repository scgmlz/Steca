// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/def/numbers.h
//! @brief     Defines NAN and INF
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

// adapted parts of https://github.com/Microsoft/GSL.git
// added some more

#ifndef NUMBERS_H
#define NUMBERS_H

#include "core/def/debug.h"
#include "core/def/macros.h"
#include <QtGlobal> // to define qreal ?

// reals

#undef NAN
#undef INF

extern qreal const NAN; // silent nan
extern qreal const INF;

#endif // NUMBERS_H

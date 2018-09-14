//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/coord_trafos.h
//! @brief     Defines function calculateAlphaBeta
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef COORD_TRAFOS_H
#define COORD_TRAFOS_H

#include "core/misc/angles.h" // no auto rm

namespace algo {

void calculateAlphaBeta(deg& alpha, deg& beta, deg tth, deg gma, deg chi, deg omg, deg phi);

} // namespace algo

#endif // COORD_TRAFOS_H

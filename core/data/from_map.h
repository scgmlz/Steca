//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/from_map.h
//! @brief     Defines namespace fromMap
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FROM_MAP_H
#define FROM_MAP_H

#include "core/raw/measurement.h"

namespace fromMap {

Range rgeGma(const Measurement* m);
Range rgeGmaFull(const Measurement* m);
Range rgeTth(const Measurement* m);

} // fromMap

#endif // FROM_MAP_H

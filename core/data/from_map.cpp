//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/from_map.cpp
//! @brief     Implements namespace fromMap
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/data/from_map.h"
#include "core/session.h"

namespace fromMap {

Range rgeGma(const Measurement* m)
{
    return gSession->angleMap.get(m->midTth()).rgeGma();
}

Range rgeGmaFull(const Measurement* m)
{
    return gSession->angleMap.get(m->midTth()).rgeGmaFull();
}

Range rgeTth(const Measurement* m)
{
    return gSession->angleMap.get(m->midTth()).rgeTth();
}

} // fromMap

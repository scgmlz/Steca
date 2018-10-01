//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/size2d.cpp
//! @brief     Implements size2d
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/typ/size2d.h"
#include <QtGlobal> // qMax

size2d operator-(const size2d& s1, const size2d& s2)
{
    int w = s1.w - s2.w;
    int h = s1.h - s2.h;
    return size2d(qMax(w, 0), qMax(h, 0));
}

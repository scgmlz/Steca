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

#include "size2d.h"
#include "core/def/comparators.h"
#include <QtGlobal> // qMax

int size2d::compare(const size2d& that) const
{
    RET_COMPARE_VALUE(w)
    RET_COMPARE_VALUE(h)
    return 0;
}

size2d operator-(const size2d& s1, const size2d& s2)
{
    int w = s1.w - s2.w;
    int h = s1.h - s2.h;
    return size2d(qMax(w, 0), qMax(h, 0));
}

EQ_NE_OPERATOR(size2d)

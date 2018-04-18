//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/def/idiomatic_for.h
//! @brief     Defines idiomatic loops (for_int, ...)
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef IDIOMATIC_FOR_H
#define IDIOMATIC_FOR_H

#define for_int(i, n) \
    for (int i = 0, i##End = (n); i < i##End; ++i)

#define for_i(n) for_int (i, n)

#define for_ij(ni, nj)  \
    for_int (i, ni)     \
        for_int (j, nj)

#endif // IDIOMATIC_FOR_H

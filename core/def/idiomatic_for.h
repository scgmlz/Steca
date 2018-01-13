// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/def/idiomatic_for.h
//! @brief     Defines idiomatic loops (for_int, ...) and ONLY_ONCE
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IDIOMATIC_FOR_H
#define IDIOMATIC_FOR_H

#include <QtGlobal> // to define Q_OS_WIN

#ifdef Q_OS_WIN

// MSVC does not handle decltype etc. well, and we disregard int/int warnings on Windows, anyway
#define for_int(i, n) for (int i = 0, i##End = (n); i < i##End; ++i)
#define for_int_down(n) for (int i = (num); i-- > 0;)

#else

#define for_int(i, n) \
    for (std::remove_const<decltype(n)>::type i = decltype(n)(0), i##End = (n); i < i##End; ++i)
#define for_int_down(n) for (std::remove_const<decltype(n)>::type i = (num); i-- > decltype(n)(0);)

#endif

#define for_i(n) for_int (i, n)

#define for_i_down(n) for_int_down (i, n)

#define for_ij(ni, nj)  \
    for_int (i, ni)     \
        for_int (j, nj)

#endif // IDIOMATIC_FOR_H

/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef DEF_ALG_H
#define DEF_ALG_H

#include <QtGlobal>

// idiomatic loops
#ifdef Q_OS_WIN

// MSVC does not handle decltype etc. well, and we disregard int/uint warnings on Windows, anyway
#define for_int(i, n) \
  for (int i = 0, i##End = (n); i < i##End; ++i)
#define for_int_down(n) \
  for (int i = (num); i-- > 0; )

#else

#define for_int(i, n) \
  for (std::remove_const<decltype(n)>::type i = decltype(n)(0), i##End = (n); i < i##End; ++i)
#define for_int_down(n) \
  for (std::remove_const<decltype(n)>::type i = (num); i-- > decltype(n)(0); )

#endif

#define for_i(n) \
  for_int (i, n)

#define for_i_down(n) \
  for_int_down (i, n)

#define for_ij(ni, nj) \
  for_int (i, ni)      \
    for_int (j, nj)

// singletons
#define ONLY_ONCE \
{ static bool once = false; if (once) return; once = true; }

#endif // DEF_ALG_H

// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      def_alg.h
//! @brief     Helper macros for defining types end else.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef DEF_ALG_H
#define DEF_ALG_H

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

#endif // DEF_MACROS_H

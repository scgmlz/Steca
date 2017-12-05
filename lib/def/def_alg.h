// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/def/def_alg.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //


#ifndef DEF_ALG_H
#define DEF_ALG_H

#include <QtGlobal>

//------------------------------------------------------------------------------
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

// code to call only once - guard
#define ONLY_ONCE \
{ static bool once = false; if (once) return; once = true; }

//------------------------------------------------------------------------------
#endif // DEF_ALG_H
// eof

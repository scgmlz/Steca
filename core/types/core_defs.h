// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_defs.h
//! @brief     Globally used definitions and includes.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CORE_DEFS_H
#define CORE_DEFS_H

#include "inc/types_inc_qt_types.h"

// common QT includes - everyone needs them
#include <QtGlobal>
#include <QSharedPointer>

#include <QString>
#include <QStringBuilder>

// string support
typedef QString     str;        ///< a short alias for the QString class
typedef str const&  rcstr;      ///< a reference to a string constant
typedef char const* pcstr;      ///< C-style (zero-byte-terminated) string

extern  str const   EMPTY_STR;  ///< an empty string

/// idiomatic loops
#define for_int(i, n) \
  for (std::remove_const<decltype(n)>::type i = decltype(n)(0), i##End = (n); i < i##End; ++i)

#define for_int_down(n) \
  for (std::remove_const<decltype(n)>::type i = (num); i-- > decltype(n)(0); )

#define for_i(n) \
  for_int (i, n)

#define for_i_down(n) \
  for_int_down (i, n)

#define for_ij(ni, nj) \
  for_int (i, ni)      \
    for_int (j, nj)

// useful vectors
typedef vec<qreal> qreal_vec;
typedef vec<uint>  uint_vec;

// exceptions
#include <QException>

/// An exception that carries a message.
class Exception : public QException {
  SUPER(Exception, QException)
protected:
  Exception(rcstr msg, bool silent) noexcept;
public:
  Exception()                 noexcept;
  Exception(rcstr msg)        noexcept;
  Exception(Exception const&) noexcept;

  bool  silent() const noexcept { return silent_; }
  rcstr msg()    const noexcept { return msg_;    }
  pcstr what()   const noexcept;

  Exception* clone() const;
  void       raise() const;

protected:
  str        msg_;
  QByteArray msg8bit_;
  bool       silent_;
};

/// exception specification macro
#ifdef Q_OS_WIN
#define THROWS
#else
#define THROWS throw(Exception)
#endif

/// raise an exception
#define THROW(msg)     throw Exception(msg)
#define THROW_SILENT() throw Exception()

/// run-time condition checking
#define RUNTIME_CHECK(test, msg) \
  if (!(test)) THROW(msg)

// gsl selections + debug support
#include "inc/types_inc_gsl.h"

#endif  // CORE_DEFS_H

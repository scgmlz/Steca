// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_defs.h
//! @brief     Globally used definitions and includes.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_DEFS_H
#define CORE_DEFS_H

// common QT includes - everyone needs them
#include <QtGlobal>
#include <QSharedPointer>
#include <QVector>

#include <QString>
#include <QStringBuilder>

// string support
typedef QString     str;      ///< a short alias for the QString class
typedef str const&  rcstr;    ///< a reference to a string constant
typedef char const* pcstr;    ///< C-style (zero-byte-terminated) string

typedef QStringList str_lst;  ///< a short alias for QStringList

extern  str const EMPTY_STR;  ///< an empty string (that can be returned by reference!)

/// A class definition helper that defines aliases for this and super class.
#define SUPER(cls,sup)  using thisClass = cls; using super = sup;

/// idiomatic iterations
#define for_int(i,n)  for (int i=0, i##End=(n); i<i##End; ++i)
#define for_i(n) for_int(i,n)

#define for_ij(ni,nj) for_int(i,ni) for_int(j,nj)

// useful vectors
typedef QVector<qreal> qreal_vec;
typedef QVector<uint>  uint_vec;

// conversions
qreal rad2deg(qreal rad);
qreal deg2rad(qreal deg);

// exceptions
#include <QException>

/// exception specification macro
#ifdef Q_OS_WIN
#define THROWS
#else
#define THROWS throw (Exception)
#endif

/// An exception that carries a message.
class Exception: public QException {
  SUPER(Exception,QException)
public:
  Exception(rcstr msg)        noexcept;
  Exception(Exception const&) noexcept;

  rcstr msg()  const noexcept { return msg_; }
  pcstr what() const noexcept;

  Exception *clone() const;
  void raise()       const;

protected:
  str msg_;
  QByteArray msg8bit_;
};

/// raise an exception
#define THROW(msg)  throw Exception(msg)

/// run-time condition checking
#define RUNTIME_CHECK(test,msg)   if (!(test)) THROW(msg)

// debug support
#include "core_debug.h"

#endif // CORE_DEFS_H

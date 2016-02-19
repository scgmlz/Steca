/** \file
 * Commonly used or very useful definitions.
 */

#ifndef CORE_DEFS_H
#define CORE_DEFS_H

/// Macros that enable development-related shortcuts.
/// Undefine for a release!
//#define DEVELOPMENT_JAN
//#define DEVELOPMENT_FAKE_NON_SQUARE_IMAGE

// common QT includes - used all the time
#include <QtGlobal>
#include <QSharedPointer>
#include <QVector>

/// A class definition helper: this class (thisCls) and superclass (super) access.
#define SUPER(cls,sup)  typedef cls thisCls; typedef sup super;

#include <QString>
#include <QStringBuilder>

typedef QString     str;      ///< a short alias for the string class
typedef str const&  rcstr;    ///< a reference to a string constant
typedef char const* pcstr;    ///< zero-terminated C-style string

typedef QStringList str_lst;  ///< a short alias

extern  str const EMPTY_STR;  ///< an empty string (that can be returned by reference!)

/// the idiomatic iteration over *n* items
#define for_i(n) for (int i=0, iEnd=(n); i<iEnd; ++i)

// exceptions
#include <QException>

#define THROWS throw (Exception)    ///< exception annotation macro

/// An exception that carries a message.
class Exception: public QException {
public:
  Exception(rcstr msg_)             throw(): msg(msg_)      {}
  Exception(Exception const& that)  throw(): msg(that.msg)  {}
 ~Exception()                       throw()                 {}

  Exception *clone() const;
  void raise() const;

  str msg;
};

#define THROW(msg) throw Exception(msg)   ///< raise an exception
#define RUNTIME_CHECK(test,msg) \
  if (!(test)) THROW(msg)                 ///< run-time condition checking

// debug support
#include "core_debug.h"

#endif

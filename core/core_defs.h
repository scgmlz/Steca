/** \file
 * Commonly used or very useful definitions.
 */

#ifndef CORE_DEFS_H
#define CORE_DEFS_H

/// Define this macro to enable development-related shortcuts.
/// Undefine it for a release!
#define DEVELOPMENT

// common QT includes - used in many places
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

extern  str const EMPTY_STR;  ///< an empty string that can be returned by reference

/// the idiomatic iteration over *n* items
#define for_i(n) for (int i=0, iEnd=(n); i<iEnd; ++i)

#include <QException>

#define THROWS throw (Exception)    ///< exception annotation macro

/// An exception that carries a message.
/// (Error handling is preferably to be done with exceptions.)
class Exception: public QException {
public:
  Exception(rcstr msg_) throw(): msg(msg_) {}
 ~Exception() throw() {}

//  void raise() const;
//  Exception *clone() const;

  str msg;
};

#define THROW(msg) throw Exception(msg)   ///< raise an exception
#define RUNTIME_CHECK(test,msg) \
  if (!(test)) THROW(msg)                 ///< run-time condition checking

// debug support
#include "core_debug.h"

#endif

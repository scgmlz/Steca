/** \file
 * Commonly used or very useful definitions.
 */

#ifndef CORE_DEFS_H
#define CORE_DEFS_H

/// Define this macro to enable development-related shortcuts.
/// Undefine it for a release!
#define DEVELOPMENT

#include <QtGlobal>

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
  Exception(rcstr msg_): msg(msg_) {}
  str msg;

  static void raise(rcstr msg) THROWS;
};

#define THROW(msg) Exception::raise(msg)  ///< raise an exception
#define RUNTIME_CHECK(test,msg) \
  if (!(test)) THROW(msg)                 ///< run-time condition checking

// we use shared pointer in many places
#include <QSharedPointer>

// math
qreal deg_rad(qreal);   ///< degrees <= radians
qreal rad_deg(qreal);   ///< radians <= degrees

// debug support
#include "core_debug.h"

#endif

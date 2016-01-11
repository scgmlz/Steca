// common and useful definitions

#ifndef DEFS_H
#define DEFS_H

#include "core_debug.h"

#include <QtGlobal>
#include <QException>

// strings
#include <QString>
#include <QStringBuilder>

typedef QString     str;
typedef str const&  rcstr;
typedef char const* pcstr;

typedef QStringList   str_lst;

// a class definition helper: this class and superclass access
#define SUPER(cls,sup)  typedef cls thisCls; typedef sup super;

// iteration
#define for_i(num)          for (int i=0, iEnd=(num); i<iEnd; ++i)
#define for_i_reverse(num)  for (int i=(num); i-->0; )

// exceptions

class Exc: public QException {
public:
  Exc(rcstr msg);
 ~Exc() throw ();

  str msg;

  void warn() const;
};

#define DECLARE_EXCEPTION(ExcCls)   \
class ExcCls: public Exc {          \
public:                             \
  ExcCls(rcstr msg): Exc(msg) {}    \
 ~ExcCls() throw ()           {}    \
  ExcCls* clone() const {           \
    return new ExcCls(*this);       \
  }                                 \
  void raise() const {              \
      throw *this;                  \
  }                                 \
};

DECLARE_EXCEPTION(CriticalError)
void CRITICAL_ERROR(rcstr msg) throw (CriticalError);

void warn(QObject*, rcstr msg, rcstr more = str::null);

#endif

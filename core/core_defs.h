// common and useful definitions

#ifndef CORE_DEFS_H
#define CORE_DEFS_H

#define DEVEL

#include <QtGlobal>

// strings
#include <QString>
#include <QStringBuilder>

typedef QString     str;
typedef str const&  rcstr;
typedef char const* pcstr;

typedef QStringList str_lst;

extern  str const NULL_STR;

// error handling
#include <QException>

class Exception: public QException {
public:
  Exception(rcstr msg_): msg(msg_) {}

  str msg;
};

#define THROWS throw (Exception)
#define THROW(msg) raiseError(msg)
#define RUNTIME_CHECK(test,msg)  if (!(test)) THROW(msg)

void raiseError(rcstr msg) THROWS;

// autoptr
#include <QSharedPointer>

// debug support
#include "core_debug.h"

// core-specific

namespace core {

struct interval_t { // closed interval
  interval_t(qreal low = 0, qreal hig = 0);
  qreal low, hig;

  void set(qreal);
  void set(qreal,qreal);
  void safeSet(qreal,qreal);  // in the right order

  void include(qreal);
  bool contains(qreal val) const { return low <= val && val <= hig; }
};

struct borders_t {
  interval_t
    gamma,
    tth_regular,
    tth_gamma0; // at gamma=0
};

}

#endif

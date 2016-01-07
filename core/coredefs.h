// common and useful definitions

#ifndef CORE_COREDEFS_H
#define CORE_COREDEFS_H

#define DEVEL

#ifdef BUILDING_LIBRARY
#define QSHARED_EXPORT Q_DECL_EXPORT
#else
#define QSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QtGlobal>
#include <QString>
#include <QStringBuilder>
#include <QException>

namespace core {

typedef QString     str;
typedef str const&  rcstr;

class QSHARED_EXPORT Exception: public QException {
public:
  Exception(rcstr msg_): msg(msg_) {}

  const char* what() const throw () {
    return msg.toLocal8Bit().data();
  }

  str msg;
};

#define THROWS throw (Exception)

void raiseError(rcstr msg) THROWS;

#define THROW(msg) raiseError(msg);

}

#include "debug.h"

#endif

#include "core_defs.h"

static_assert (QT_VERSION >= QT_VERSION_CHECK(5,5,0),"written for Qt >= 5.5, have " QT_VERSION_STR);

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

str const EMPTY_STR;

Exception* Exception::clone() const {
  return new Exception(*this);
}

void Exception::raise() const {
  throw *this;
}

// eof

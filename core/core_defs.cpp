#include "core_defs.h"

static_assert (QT_VERSION >= 0x050501,"written for Qt >= 5.5");

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

str const EMPTY_STR;

void Exception::raise(rcstr msg) THROWS {
  throw Exception(msg);
}

// eof

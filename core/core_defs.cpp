#include "core_defs.h"
#include <math.h>

static_assert (QT_VERSION >= 0x050501,"written for Qt >= 5.5");

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

str const EMPTY_STR;

void Exception::raise(rcstr msg) THROWS {
  throw Exception(msg);
}

qreal deg_rad(qreal rad) {
  return rad * (180 / M_PI);
}

qreal rad_deg(qreal deg) {
  return deg * (M_PI / 180);
}

// eof

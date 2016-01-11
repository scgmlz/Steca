#include "core_defs.h"

void raiseError(rcstr msg) THROWS {
  throw Exception(msg);
}

// eof

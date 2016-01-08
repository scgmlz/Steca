#include "core_defs.h"

namespace core {

void raiseError(rcstr msg) THROWS {
  throw Exception(msg);
}

}

// eof

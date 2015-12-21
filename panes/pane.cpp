#include "pane.h"

Pane::Pane(rcstr title): super(title) {
  auto sp = sizePolicy();
  setSizePolicy(sp);
}

Pane::~Pane() {
}

// eof

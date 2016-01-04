#include "split.h"

Split::Split(Qt::Orientation orientation) {
  setLayout((box = boxLayout(orientation)));
}

// eof

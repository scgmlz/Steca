#include "editcell.h"

EditCell::EditCell(): super() {
  int h = super::sizeHint().height();
  setMaximumWidth(2*h);
}

// eof

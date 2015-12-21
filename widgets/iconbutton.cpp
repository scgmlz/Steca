#include "iconbutton.h"

IconButton::IconButton(rcstr fileName): super(QIcon(fileName),nullstr) {
  int h = super::sizeHint().height();
  setMaximumSize(h,h);
}

// eof

#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include "defs.h"
#include <QPushButton>

class IconButton: public QPushButton {
  SUPER(IconButton,QPushButton)
public:
  IconButton(rcstr fileName);
};

#endif

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "panel.h"

namespace panel {

class Background: public Panel {
  SUPER(Background,Panel)
public:
  Background(MainWin&);
};

}

#endif

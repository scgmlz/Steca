#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "panel.h"

namespace panel {

class Background: public BoxPanel {
  SUPER(Background,BoxPanel)
public:
  Background(MainWin&,Session&);
};

}

#endif

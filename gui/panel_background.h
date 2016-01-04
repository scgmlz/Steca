#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "panel.h"

class PanelBackground: public Panel {
  SUPER(PanelBackground,Panel)
public:
  PanelBackground(MainWin&);
};

#endif

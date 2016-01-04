#ifndef PANEL_DIFFRACTOGRAM_H
#define PANEL_DIFFRACTOGRAM_H

#include "panel.h"

class PanelDiffractogram: public Panel {
  SUPER(PanelDiffractogram,Panel)
public:
  PanelDiffractogram(MainWin&);
};

#endif

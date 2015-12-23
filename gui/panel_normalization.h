#ifndef PANEL_NORMALIZATION_H
#define PANEL_NORMALIZATION_H

#include "panel.h"

class PanelNormalization: public Panel {
  SUPER(PanelNormalization,Panel)
public:
  PanelNormalization(MainWin&);
};

#endif

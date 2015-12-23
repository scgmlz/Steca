#ifndef PANEL_FILES_H
#define PANEL_FILES_H

#include "panel.h"

class PanelFiles: public Panel {
  SUPER(PanelFiles,Panel)
public:
  PanelFiles(MainWin&);
};

#endif

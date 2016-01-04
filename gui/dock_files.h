#ifndef DOCK_FILES_H
#define DOCK_FILES_H

#include "panel.h"

class DockFiles: public Docking {
  SUPER(DockFiles,Docking)
public:
  DockFiles(MainWin&);
};

#endif

#ifndef DOCK_INFO_H
#define DOCK_INFO_H

#include "panel.h"

class DockInfo: public Docking {
  SUPER(DockInfo,Docking)
public:
  DockInfo(MainWin&);
};

#endif

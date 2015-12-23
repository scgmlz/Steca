#ifndef DOCK_INFO_H
#define DOCK_INFO_H

#include "defs.h"
#include <QDockWidget>

class MainWin;

class DockInfo: public QDockWidget {
  SUPER(DockInfo,QDockWidget)
public:
  DockInfo(MainWin&);
};

#endif

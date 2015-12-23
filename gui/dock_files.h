#ifndef DOCK_FILES_H
#define DOCK_FILES_H

#include "defs.h"
#include <QDockWidget>

class MainWin;

class DockFiles: public QDockWidget {
  SUPER(DockFiles,QDockWidget)
public:
  DockFiles(MainWin&);
};

#endif

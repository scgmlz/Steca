#ifndef DOCK_FILES_H
#define DOCK_FILES_H

#include "defs.h"
#include <QDockWidget>

class DockFiles: public QDockWidget {
  SUPER(DockFiles,QDockWidget)
public:
  DockFiles();
};

#endif

#include "dock_info.h"

DockInfo::DockInfo(MainWin&) {
  setObjectName("DockInfo");
  setWindowTitle("Info");

  auto w = new QWidget;
  setWidget(w);
}

// eof

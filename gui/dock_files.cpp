#include "dock_files.h"
#include "layout_helpers.h"
#include "panel_files.h"
#include "panel_detector.h"
#include "panel_normalization.h"

#include <QLabel>

DockFiles::DockFiles(MainWin& mainWin) {
  setObjectName("DockFiles");
  setWindowTitle("Files");

  auto w = new QWidget;
  setWidget(w);

  auto v = vbox();
  w->setLayout(v);

  v->addWidget(new PanelFiles(mainWin));
  v->addWidget(new PanelDetector(mainWin));
  v->addWidget(new PanelNormalization(mainWin));
}

// eof

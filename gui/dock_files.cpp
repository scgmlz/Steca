#include "dock_files.h"
#include "layout_helpers.h"
#include "panel_files.h"
#include "panel_detector.h"
#include "panel_normalization.h"

#include <QLabel>

DockFiles::DockFiles() {
  setObjectName("DockFiles");
  setWindowTitle("Files");

  auto w = new QWidget;
  setWidget(w);

  auto v = vbox();
  w->setLayout(v);

  v->addWidget(new PanelFiles);
  v->addWidget(new PanelDetector);
  v->addWidget(new PanelNormalization);
}

// eof

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

  auto pf = new PanelFiles;
  v->addWidget(pf);
//  pf->noMargin();

  auto pd = new PanelDetector;
  v->addWidget(pd);
//  pf->noMargin();

  auto pn = new PanelNormalization;
  v->addWidget(pn);
}

// eof

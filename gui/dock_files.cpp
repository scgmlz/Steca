#include "dock_files.h"
#include "panel_files.h"
#include "panel_detector.h"
#include "panel_normalization.h"

DockFiles::DockFiles(MainWin& mainWin): super("DockFiles","Files") {
  auto panelFiles = new PanelFiles(mainWin);
  panelFiles->setStretchFactors(0,1);
  box->addWidget(panelFiles);
  box->addWidget(new PanelDetector(mainWin));
  box->addWidget(new PanelNormalization(mainWin));
}

// eof

#include "split_files.h"
#include "mainwin.h"
#include "panels/files.h"
#include "panels/detector.h"
#include "panels/normalization.h"

SplitFiles::SplitFiles(MainWin& mainWin): super(Qt::Vertical) {
  auto panelFiles = new panel::Files(mainWin);
  panelFiles->setVerticalStretch(1);
  box->addWidget(panelFiles);
  box->addWidget(new panel::Detector(mainWin));
  box->addWidget(new panel::Normalization(mainWin));
}

// eof

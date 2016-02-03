#include "split_files.h"
#include "mainwin.h"
#include "session.h"
#include "panel/files.h"
#include "panel/detector.h"
#include "panel/normalization.h"

SplitFiles::SplitFiles(MainWin& mainWin,Session& session): super(Qt::Vertical) {
  auto panelFiles = new panel::Files(mainWin,session);
  panelFiles->setVerticalStretch(1);
  box->addWidget(panelFiles);
  box->addWidget(new panel::Detector(mainWin,session));
  box->addWidget(new panel::Normalization(mainWin,session));
}

// eof

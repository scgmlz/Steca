#include "split_files.h"
#include "mainwin.h"
#include "session.h"
#include "panel/files.h"
#include "panel/datasets.h"
#include "panel/normalization.h"

//------------------------------------------------------------------------------

SplitFiles::SplitFiles(MainWin& mainWin,Session& session): super(Qt::Vertical) {
  auto panelFiles    = new panel::Files(mainWin,session);
  auto panelDatasets = new panel::Datasets(mainWin,session);
  box->addWidget(panelFiles);
  box->addWidget(panelDatasets);
  panelFiles->setVerticalStretch(1);
  panelDatasets->setVerticalStretch(1);
  box->addWidget(new panel::Normalization(mainWin,session));
}

//------------------------------------------------------------------------------
// eof

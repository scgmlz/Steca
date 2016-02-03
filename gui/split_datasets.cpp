#include "split_datasets.h"
#include "mainwin.h"
#include "session.h"
#include "panel/datasets.h"

SplitDatasets::SplitDatasets(MainWin& mainWin,Session& session): super(Qt::Horizontal) {
  box->addWidget(new panel::Datasets(mainWin,session));
}

// eof

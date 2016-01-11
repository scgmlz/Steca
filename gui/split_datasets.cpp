#include "split_datasets.h"
#include "mainwin.h"
#include "panels/datasets.h"

SplitDatasets::SplitDatasets(MainWin& mainWin): super(Qt::Horizontal) {
  box->addWidget(new panel::Datasets(mainWin));
}

// eof

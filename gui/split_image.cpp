#include "split_image.h"
#include "mainwin.h"
#include "panels/dataset.h"
#include "panels/datasetinfo.h"

SplitImage::SplitImage(MainWin& mainWin,Session& session): super(Qt::Horizontal) {
  box->addWidget(new panel::Dataset(mainWin,session));
  box->addWidget(new panel::DatasetInfo(mainWin,session));
  box->setStretch(1,1);
}

// eof

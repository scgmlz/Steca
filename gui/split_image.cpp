#include "split_image.h"
#include "mainwin.h"
#include "panels/dataset.h"
#include "panels/datasetinfo.h"

SplitImage::SplitImage(MainWin& mainWin): super(Qt::Horizontal) {
  box->addWidget(new panel::Dataset(mainWin));
  box->addWidget(new panel::DatasetInfo(mainWin));
  box->setStretch(1,1);
}

// eof

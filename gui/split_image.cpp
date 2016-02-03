#include "split_image.h"
#include "mainwin.h"
#include "session.h"
#include "panel/dataset.h"
#include "panel/datasetinfo.h"

SplitImage::SplitImage(MainWin& mainWin,Session& session): super(Qt::Horizontal) {
  box->addWidget(new panel::Dataset(mainWin,session));
  box->addWidget(new panel::DatasetInfo(mainWin,session));
  box->setStretch(1,1);
}

// eof

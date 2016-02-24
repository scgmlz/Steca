#include "split_image.h"
#include "mainwin.h"
#include "session.h"
#include "panel/dataset.h"
#include "panel/datasetinfo.h"

//------------------------------------------------------------------------------

SplitImage::SplitImage(MainWin& mainWin,Session& session): super(Qt::Horizontal) {
  auto *options = new panel::DatasetOptions(mainWin,session);
  auto *dataset = new panel::Dataset(mainWin,session);
  connect(options, &panel::DatasetOptions::imageScale, dataset, &panel::Dataset::setImageScale);
  box->addWidget(options);
  box->addWidget(dataset);
  box->addWidget(new panel::DatasetInfo(mainWin,session));
  box->setStretch(1,1);
}

//------------------------------------------------------------------------------
// eof

#include "split_images.h"
#include "mainwin.h"
#include "panels/datasets.h"

SplitImages::SplitImages(MainWin& mainWin): super(Qt::Horizontal) {
  box->addWidget(new panel::Datasets(mainWin));
}

// eof

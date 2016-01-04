#include "split_images.h"
#include "mainwin.h"
#include "panels/images.h"

SplitImages::SplitImages(MainWin& mainWin): super(Qt::Horizontal) {
  box->addWidget(new panel::Images(mainWin));
}

// eof

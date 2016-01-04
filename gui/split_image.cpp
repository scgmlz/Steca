#include "split_image.h"
#include "mainwin.h"
#include "panels/image.h"
#include "panels/imageinfo.h"

SplitImage::SplitImage(MainWin& mainWin): super(Qt::Horizontal) {
  box->addWidget(new panel::Image(mainWin));
  box->addWidget(new panel::ImageInfo(mainWin));
}

// eof

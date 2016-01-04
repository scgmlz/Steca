#include "split_info.h"
#include "mainwin.h"
#include "panels/imageinfo.h"
#include "panels/reflectioninfo.h"

SplitInfo::SplitInfo(MainWin& mainWin): super(Qt::Vertical) {
  box->addWidget(new panel::ImageInfo(mainWin));
  box->addWidget(new panel::ReflectionInfo(mainWin));
}

// eof

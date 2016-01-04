#include "split_diffractogram.h"
#include "mainwin.h"
#include "panels/diffractogram.h"
#include "panels/reflectioninfo.h"

SplitDiffractogram::SplitDiffractogram(MainWin& mainWin): super(Qt::Horizontal) {
  auto diffractogram = new panel::Diffractogram(mainWin);
  diffractogram->setHorizontalStretch(1);
  box->addWidget(diffractogram);
  box->addWidget(new panel::ReflectionInfo(mainWin));
}

// eof

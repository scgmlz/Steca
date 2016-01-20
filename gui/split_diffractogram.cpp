#include "split_diffractogram.h"
#include "mainwin.h"
#include "panels/diffractogram.h"
#include "panels/reflectioninfo.h"

SplitDiffractogram::SplitDiffractogram(MainWin& mainWin,Session& session): super(Qt::Horizontal) {
  auto diffractogram = new panel::Diffractogram(mainWin,session);
  diffractogram->setHorizontalStretch(1);
  box->addWidget(diffractogram);
  box->addWidget(new panel::ReflectionInfo(mainWin,session));
}

// eof

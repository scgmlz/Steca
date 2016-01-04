#include "split_reflections.h"
#include "mainwin.h"
#include "panels/background.h"
#include "panels/reflections.h"

SplitReflections::SplitReflections(MainWin& mainWin): super(Qt::Vertical) {
  box->addWidget(new panel::Background(mainWin));
  box->addWidget(new panel::Reflections(mainWin));
}

// eof

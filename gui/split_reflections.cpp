#include "split_reflections.h"
#include "mainwin.h"
#include "panels/background.h"
#include "panels/reflections.h"

SplitReflections::SplitReflections(MainWin& mainWin,Session& session): super(Qt::Vertical) {
  box->addWidget(new panel::Background(mainWin,session));
  box->addWidget(new panel::Reflections(mainWin,session));
}

// eof

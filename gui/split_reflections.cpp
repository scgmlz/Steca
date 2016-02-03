#include "split_reflections.h"
#include "mainwin.h"
#include "session.h"
#include "panel/background.h"
#include "panel/reflections.h"

SplitReflections::SplitReflections(MainWin& mainWin,Session& session): super(Qt::Vertical) {
  box->addWidget(new panel::Background(mainWin,session));
  box->addWidget(new panel::Reflections(mainWin,session));
}

// eof

#include "normalization.h"
#include "gui_helpers.h"

namespace panel {

Normalization::Normalization(MainWin& mainWin,Session& session)
: super("Normalization",mainWin,session,Qt::Vertical) {
  box->addWidget(radioButton("None"));
  box->addWidget(radioButton("To deltatime"));
  box->addWidget(radioButton("To deltaMonitor"));
  box->addWidget(radioButton("To background"));
  box->addStretch();
}

}

// eof

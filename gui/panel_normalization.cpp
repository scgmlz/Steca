#include "panel_normalization.h"
#include "gui_helpers.h"

PanelNormalization::PanelNormalization(MainWin& mainWin)
: super(mainWin,Qt::Vertical,"Normalization") {

  box->addWidget(radioButton("None"));
  box->addWidget(radioButton("To deltatime"));
  box->addWidget(radioButton("To deltaMonitor"));
  box->addWidget(radioButton("To background"));
  box->addStretch();
}

// eof

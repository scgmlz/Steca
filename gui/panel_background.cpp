#include "panel_background.h"

PanelBackground::PanelBackground(MainWin& mainWin)
: super(mainWin,Qt::Horizontal,"Background") {

  box->addWidget(iconButton(":/icon/background"));
  box->addWidget(label("Degree:"));
  box->addWidget(editCell());
  box->addWidget(iconButton(":/icon/eye"));
}

// eof

#include "background.h"
#include "mainwin.h"

namespace panel {

Background::Background(MainWin& mainWin)
: super(mainWin,"Background",Qt::Horizontal) {

  box->addWidget(iconButton(mainWin.actBackgroundBackground));
  box->addWidget(label("Degree:"));
  box->addWidget(editCell(4));
  box->addWidget(iconButton(mainWin.actBackgroundEye));
}

}

// eof

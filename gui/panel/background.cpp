#include "background.h"
#include "mainwin.h"

namespace panel {

Background::Background(MainWin& mainWin,Session& session)
: super("Background",mainWin,session,Qt::Horizontal) {

  box->addWidget(iconButton(mainWin.actBackgroundBackground));
  box->addWidget(label("Degree:"));
  box->addWidget(spinCell(4,0,99));
  box->addStretch();
  box->addWidget(iconButton(mainWin.actBackgroundEye));
}

}

// eof

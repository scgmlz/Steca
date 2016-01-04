#include "panel.h"
#include "mainwin.h"
#include "gui_helpers.h"
#include <QGroupBox>
#include <QToolButton>

Docking::Docking(rcstr objectName, rcstr windowTitle) {
  setObjectName(objectName);
  setWindowTitle(windowTitle);

  auto w = new QWidget;
  setWidget(w);

  box = vbox(MARGIN);
  w->setLayout(box);
}

//-----------------------------------------------------------------------------

Panel::Panel(MainWin& mainWin_,Qt::Orientation orientation)
: Panel(mainWin_, orientation, nullptr) {
}

Panel::Panel(MainWin& mainWin_, Qt::Orientation orientation, rcstr groupBox)
  : Panel(mainWin_, orientation, &groupBox) {
}

void Panel::setStretchFactors(int horizontal, int vertical) {
  auto sp = sizePolicy();
  sp.setHorizontalStretch(horizontal);
  sp.setVerticalStretch(vertical);
  setSizePolicy(sp);
}

Panel::Panel(MainWin& mainWin_, Qt::Orientation orientation, const str* groupBox)
: mainWin(mainWin_) {

  setStretchFactors(0,0);

  box = boxLayout(orientation);
  // white space around panels provided elsewhere
  box->setContentsMargins(0,0,0,0);

  if (groupBox) {
    setLayout(vbox());
    auto gr = new QGroupBox(*groupBox);
    layout()->addWidget(gr);
    gr->setLayout(box);
  } else {
    setLayout(box);
  }
}

// eof

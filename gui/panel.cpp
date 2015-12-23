#include "panel.h"
#include "mainwin.h"
#include "layout_helpers.h"

#include <QLayout>
#include <QGroupBox>

Panel::Panel(MainWin& mainWin_): mainWin(mainWin_) {
  setLayout((v = vbox()));
  // white space around panels provided elsewhere
  v->setContentsMargins(0,0,0,0);
}

void Panel::addGroupBox(rcstr title) {
  v->addWidget(new QGroupBox(title));
}

// eof

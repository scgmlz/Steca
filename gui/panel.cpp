#include "panel.h"
#include "mainwin.h"
#include "layout_helpers.h"

#include <QLayout>
#include <QGroupBox>

//-----------------------------------------------------------------------------

Panel::Panel(MainWin& mainWin_): mainWin(mainWin_) {
  setLayout((v = vbox()));
  // white space around panels provided elsewhere
  v->setContentsMargins(0,0,0,0);
}

void Panel::addGroupBox(rcstr title) {
  v->addWidget(new QGroupBox(title));
}

//-----------------------------------------------------------------------------

Button::Button(rcstr text, rcstr toolTip): super(text) {
  setToolTip(toolTip);
}

Button::Button(QIcon const& icon, rcstr toolTip): super(icon,nullstr) {
  setToolTip(toolTip);
}

IconButton::IconButton(rcstr fileName, rcstr toolTip): super(QIcon(fileName),toolTip) {
  // make square
  int h = super::sizeHint().height();
  setMaximumSize(h,h);

  if (!toolTip.isEmpty()) setToolTip(toolTip);
}

// eof

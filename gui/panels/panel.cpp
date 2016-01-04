#include "panel.h"
#include "mainwin.h"

Panel::Panel(MainWin& mainWin_, rcstr title, Qt::Orientation orientation)
: super(title), mainWin(mainWin_) {
  setLayout((box = boxLayout(orientation)));
  //  box->setContentsMargins(0,0,0,0); TODO
}

void Panel::setHorizontalStretch(int stretch) {
  auto sp = sizePolicy();
  sp.setHorizontalStretch(stretch);
  setSizePolicy(sp);
}

void Panel::setVerticalStretch(int stretch) {
  auto sp = sizePolicy();
  sp.setVerticalStretch(stretch);
  setSizePolicy(sp);
}

void Panel::setStretch(int horizontal, int vertical) {
  auto sp = sizePolicy();
  sp.setHorizontalStretch(horizontal);
  sp.setVerticalStretch(vertical);
  setSizePolicy(sp);
}

// eof

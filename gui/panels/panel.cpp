#include "panel.h"
#include "mainwin.h"

BasicPanel::BasicPanel(MainWin& mainWin_, rcstr title)
: super(title), mainWin(mainWin_) {
}

void BasicPanel::setHorizontalStretch(int stretch) {
  auto sp = sizePolicy();
  sp.setHorizontalStretch(stretch);
  setSizePolicy(sp);
}

void BasicPanel::setVerticalStretch(int stretch) {
  auto sp = sizePolicy();
  sp.setVerticalStretch(stretch);
  setSizePolicy(sp);
}

void BasicPanel::setStretch(int horizontal, int vertical) {
  auto sp = sizePolicy();
  sp.setHorizontalStretch(horizontal);
  sp.setVerticalStretch(vertical);
  setSizePolicy(sp);
}

BoxPanel::BoxPanel(MainWin& mainWin, rcstr title, Qt::Orientation orientation)
: super(mainWin,title) {
  setLayout((box = boxLayout(orientation)));
}

GridPanel::GridPanel(MainWin& mainWin, rcstr title)
: super(mainWin,title) {
  setLayout((grid = gridLayout()));
}

// eof

#include "panel.h"
#include "mainwin.h"

namespace panel {

BasicPanel::BasicPanel(rcstr title, MainWin& mainWin_, Session& session_)
: super(title), mainWin(mainWin_), session(session_) {
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

BoxPanel::BoxPanel(rcstr title,MainWin& mainWin,Session& session, Qt::Orientation orientation)
: super(title,mainWin,session) {
  setLayout((box = boxLayout(orientation)));
}

GridPanel::GridPanel(rcstr title,MainWin& mainWin,Session& session)
: super(title,mainWin,session) {
  setLayout((grid = gridLayout()));
}

}

// eof

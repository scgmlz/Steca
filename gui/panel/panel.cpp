#include "panel.h"
#include "mainwin.h"

namespace panel {
//-----------------------------------------------------------------------------

HubListView::HubListView(TheHub& theHub_): theHub(theHub_) {
}

//------------------------------------------------------------------------------

BasicPanel::BasicPanel(rcstr title, TheHub& theHub_)
: super(title), theHub(theHub_) {
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

BoxPanel::BoxPanel(rcstr title, TheHub& theHub, Qt::Orientation orientation)
: super(title,theHub) {
  setLayout((box = boxLayout(orientation)));
}

GridPanel::GridPanel(rcstr title, TheHub& theHub)
: super(title,theHub) {
  setLayout((grid = gridLayout()));
}

//------------------------------------------------------------------------------
}
// eof

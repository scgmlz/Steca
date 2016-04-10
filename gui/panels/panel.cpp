// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "panel.h"
#include "mainwin.h"

namespace gui { namespace panel {
//-----------------------------------------------------------------------------

HubListView::HubListView(TheHub& theHub): RefHub(theHub) {
}

//------------------------------------------------------------------------------

BasicPanel::BasicPanel(rcstr title, TheHub& theHub): super(title), RefHub(theHub) {
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

//------------------------------------------------------------------------------

BoxPanel::BoxPanel(rcstr title, TheHub& theHub, Qt::Orientation orientation)
: super(title,theHub) {
  setLayout((_box = boxLayout(orientation)));
}

//------------------------------------------------------------------------------

GridPanel::GridPanel(rcstr title, TheHub& theHub)
: super(title,theHub) {
  setLayout((_grid = gridLayout()));
}

//------------------------------------------------------------------------------

TabsPanel::Tab::Tab(Qt::Orientation orientation) {
  setLayout((box = boxLayout(orientation)));
}

TabsPanel::TabsPanel(TheHub& theHub): RefHub(theHub) {
}

TabsPanel::Tab& TabsPanel::addTab(rcstr title, Qt::Orientation orientation) {
  auto tab = new Tab(orientation);
  super::addTab(tab,title);
  return *tab;
}

//------------------------------------------------------------------------------
}}
// eof

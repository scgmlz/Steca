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

ListView::ListView(TheHub& hub): RefHub(hub) {
}

void ListView::update(models::TableModel& model) {
  int row = currentIndex().row();
  model.signalReset();
  setCurrentIndex(model.index(qMin(row,model.rowCount()-1),0));
}

//------------------------------------------------------------------------------

BasicPanel::BasicPanel(rcstr title, TheHub& hub): super(title), RefHub(hub) {
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

BoxPanel::BoxPanel(rcstr title, TheHub& hub, Qt::Orientation orientation)
: super(title,hub) {
  setLayout((box_ = boxLayout(orientation)));
}

//------------------------------------------------------------------------------

GridPanel::GridPanel(rcstr title, TheHub& hub)
: super(title,hub) {
  setLayout((grid_ = gridLayout()));
}

//------------------------------------------------------------------------------

TabsPanel::Tab::Tab(Qt::Orientation orientation) {
  setLayout((box_ = boxLayout(orientation)));
}

TabsPanel::TabsPanel(TheHub& hub): RefHub(hub) {
}

TabsPanel::Tab& TabsPanel::addTab(rcstr title, Qt::Orientation orientation) {
  auto tab = new Tab(orientation);
  super::addTab(tab,title);
  return *tab;
}

//------------------------------------------------------------------------------
}}
// eof

// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "panel.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

BasicPanel::BasicPanel(TheHub& hub, rcstr title) : super(title), RefHub(hub) {
}

BasicPanel::BasicPanel(TheHub& hub) : thisClass(hub, EMPTY_STR) {
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

BoxPanel::BoxPanel(TheHub& hub, Qt::Orientation o)
: thisClass(hub, EMPTY_STR, o) {
}

BoxPanel::BoxPanel(TheHub& hub, rcstr title, Qt::Orientation orientation)
: super(hub, title)
{
  setLayout((box_ = boxLayout(orientation)));
}

//------------------------------------------------------------------------------

GridPanel::GridPanel(TheHub& hub) : thisClass(hub, EMPTY_STR) {
}

GridPanel::GridPanel(TheHub& hub, rcstr title) : super(hub, title) {
  setLayout((grid_ = gridLayout()));
}

//------------------------------------------------------------------------------

Tab::Tab(Qt::Orientation orientation) {
  setLayout((box_ = boxLayout(orientation)));
}

TabsPanel::TabsPanel(TheHub& hub) : RefHub(hub) {}

Tab& TabsPanel::addTab(rcstr title, Qt::Orientation orientation) {
  auto tab = new Tab(orientation);
  super::addTab(tab, title);
  return *tab;
}

Tab& TabsPanel::tab(uint i) {
  EXPECT((int)i < count())
  ENSURE(dynamic_cast<Tab*>(widget(i)))
  return *static_cast<Tab*>(widget(i));
}

//------------------------------------------------------------------------------
}}
// eof

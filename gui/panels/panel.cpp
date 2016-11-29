/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "panel.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

PanelWidget::PanelWidget(TheHub& hub, Qt::Orientation orientation) : RefHub(hub) {
  setLayout((box_ = boxLayout(orientation)));
}

//------------------------------------------------------------------------------

BasicPanel::BasicPanel(TheHub& hub, rcstr title) : super(title), RefHub(hub) {
}

BasicPanel::BasicPanel(TheHub& hub) : Cls(hub, EMPTY_STR) {
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
: Cls(hub, EMPTY_STR, o) {
}

BoxPanel::BoxPanel(TheHub& hub, rcstr title, Qt::Orientation orientation)
: super(hub, title)
{
  setLayout((box_ = boxLayout(orientation)));
}

//------------------------------------------------------------------------------

GridPanel::GridPanel(TheHub& hub) : Cls(hub, EMPTY_STR) {
}

GridPanel::GridPanel(TheHub& hub, rcstr title) : super(hub, title) {
  setLayout((grid_ = gridLayout()));
}

//------------------------------------------------------------------------------

Tab::Tab(Qt::Orientation orientation) {
  setLayout((box_ = boxLayout(orientation)));
}

TabsPanel::TabsPanel(TheHub& hub) : RefHub(hub) {
  setTabPosition(TabsPanel::North);
}

//------------------------------------------------------------------------------

Tab& TabsPanel::addTab(rcstr title, Qt::Orientation orientation) {
  auto tab = new Tab(orientation);
  super::addTab(tab, title);
  return *tab;
}

Tab& TabsPanel::tab(uint i) {
  EXPECT(to_i(i) < count())
  ENSURE(dynamic_cast<Tab*>(widget(to_i(i))))
      return *static_cast<Tab*>(widget(to_i(i)));
}

//------------------------------------------------------------------------------
}}
// eof

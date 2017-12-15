// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/panel.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "panel.h"
#include "typ/str.h"

namespace gui {
namespace panel {

PanelWidget::PanelWidget(TheHub& hub, Qt::Orientation orientation) : RefHub(hub) {
    setLayout((box_ = boxLayout(orientation)));
}


BasicPanel::BasicPanel(TheHub& hub) : BasicPanel(hub, "") {}

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

BoxPanel::BoxPanel(TheHub& hub, Qt::Orientation o) : BoxPanel(hub, "", o) {}

BoxPanel::BoxPanel(TheHub& hub, rcstr title, Qt::Orientation orientation) : BasicPanel(hub, title) {
    setLayout((box_ = boxLayout(orientation)));
}


GridPanel::GridPanel(TheHub& hub) : GridPanel(hub, "") {}

GridPanel::GridPanel(TheHub& hub, rcstr title) : BasicPanel(hub, title) {
    setLayout((grid_ = gridLayout()));
}


Tab::Tab(Qt::Orientation orientation) {
    setLayout((box_ = boxLayout(orientation)));
}


TabsPanel::TabsPanel(TheHub& hub) : RefHub(hub) {
    setTabPosition(TabsPanel::North);
}

Tab& TabsPanel::addTab(rcstr title, Qt::Orientation orientation) {
    auto tab = new Tab(orientation);
    QTabWidget::addTab(tab, title);
    return *tab;
}

Tab& TabsPanel::tab(uint i) {
    debug::ensure(to_i(i) < count());
    debug::ensure(dynamic_cast<Tab*>(widget(to_i(i))));
    return *static_cast<Tab*>(widget(to_i(i)));
}

} // namespace panel
} // namespace gui

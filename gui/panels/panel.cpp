// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/panel.cpp
//! @brief     Implements classes GridPanel, Tab, TabsPanel
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "def/numbers.h"
#include "panel.h"
#include "widgets/widget_makers.h"


namespace gui {
namespace panel {

// ************************************************************************** //
//  class Tab
// ************************************************************************** //

Tab::Tab(Qt::Orientation orientation) {
    setLayout((box_ = boxLayout(orientation)));
}

// ************************************************************************** //
//  class TabsPanel
// ************************************************************************** //

TabsPanel::TabsPanel() {
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

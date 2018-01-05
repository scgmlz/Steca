// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/panel.cpp
//! @brief     Implements classes GridPanel, Tab, TabsPanel
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "def/numbers.h"
#include "panel.h"
#include "widgets/widget_makers.h"

// ************************************************************************** //
//  class Tab
// ************************************************************************** //

PTab::PTab(Qt::Orientation orientation) {
    setLayout((box_ = boxLayout(orientation)));
}

// ************************************************************************** //
//  class TabsPanel
// ************************************************************************** //

TabsPanel::TabsPanel() {
    setTabPosition(TabsPanel::North);
}

PTab& TabsPanel::addTab(rcstr title, Qt::Orientation orientation) {
    auto tab = new PTab(orientation);
    QTabWidget::addTab(tab, title);
    return *tab;
}

PTab& TabsPanel::tab(uint i) {
    debug::ensure(to_i(i) < count());
    debug::ensure(dynamic_cast<PTab*>(widget(to_i(i))));
    return *static_cast<PTab*>(widget(to_i(i)));
}

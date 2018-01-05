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

BoxWidget* wmaker::newTab(QTabWidget* panel, rcstr title) {
    auto ret = new BoxWidget(Qt::Vertical);
    panel->addTab(ret, title);
    return ret;
}

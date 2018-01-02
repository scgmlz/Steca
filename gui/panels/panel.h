// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/panel.h
//! @brief     Defines classes Tab, TabsPanel
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PANEL_H
#define PANEL_H

#include "typ/str.h"
#include <QComboBox>
#include <QGridLayout>
#include <QToolButton>
#include <QRadioButton>
#include <QSpinBox>

namespace gui {
namespace panel {

//! A tabbed panel
class Tab : public QWidget {  // TODO: better names for panel::Tab and output::Tab
public:
    Tab(Qt::Orientation);
    QBoxLayout& box() const { return *box_; }

protected:
    QBoxLayout* box_;
};


//!
class TabsPanel : public QTabWidget {
public:
    TabsPanel();
    Tab& addTab(rcstr title, Qt::Orientation);
    Tab& tab(uint);
};

} // namespace panel
} // namespace gui

#endif // PANEL_H

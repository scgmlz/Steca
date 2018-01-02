// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/panel.h
//! @brief     Defines classes BasicPanel, GridPanel, Tab, TabsPanel
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
#include <QGroupBox>
#include <QToolButton>
#include <QRadioButton>
#include <QSpinBox>

namespace gui {
namespace panel {

//! Just a groupbox
class BasicPanel : public QGroupBox {
public:
    BasicPanel(rcstr title="") : QGroupBox(title) {}

    void setHorizontalStretch(int);
    void setVerticalStretch(int);
    void setStretch(int horizontal, int vertical);
};


//! A panel with grid layout
class GridPanel : public BasicPanel {
public:
    GridPanel(rcstr title="");

    QGridLayout* grid() const { return grid_; }

protected:
    QGridLayout* grid_;
};


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

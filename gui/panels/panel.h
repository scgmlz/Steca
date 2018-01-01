// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/panel.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PANEL_H
#define PANEL_H

#include "gui_helpers.h"
#include <QGroupBox>

namespace gui {
namespace panel {

//! Just a groupbox
class BasicPanel : public QGroupBox {
public:
    BasicPanel() : BasicPanel("") {}
    BasicPanel(rcstr title) : QGroupBox(title) {}

    void setHorizontalStretch(int);
    void setVerticalStretch(int);
    void setStretch(int horizontal, int vertical);
};


//! A panel with grid layout
class GridPanel : public BasicPanel {
public:
    GridPanel();
    GridPanel(rcstr title);

    GridLayout* grid() const { return grid_; }

protected:
    GridLayout* grid_;
};


//! A tabbed panel
class Tab : public QWidget {
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

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
#include "refhub.h"
#include <QGroupBox>

namespace gui {
namespace panel {

//! Just a groupbox
class BasicPanel : public QGroupBox {
public:
    BasicPanel(TheHub& hub) : BasicPanel(hub, "") {}
    BasicPanel(TheHub& hub, rcstr title) : QGroupBox(title), hub_(hub) {}

    void setHorizontalStretch(int);
    void setVerticalStretch(int);
    void setStretch(int horizontal, int vertical);

protected:
    TheHub& hub_;
};


//! A panel with grid layout
class GridPanel : public BasicPanel {
public:
    GridPanel(TheHub&);
    GridPanel(TheHub&, rcstr title);

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
class TabsPanel : public QTabWidget, protected RefHub {
public:
    TabsPanel(TheHub&);

    Tab& addTab(rcstr title, Qt::Orientation);
    Tab& tab(uint);
};

} // namespace panel
} // namespace gui

#endif // PANEL_H

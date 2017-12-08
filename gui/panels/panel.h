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
#include "typ/str.h"
#include <QGroupBox>

namespace models {
}

namespace gui {
namespace panel {

// REVIEW still needed?

// Just a widget
class PanelWidget : public QWidget, protected RefHub {
private:
    using super = QWidget;
public:
    PanelWidget(TheHub&, Qt::Orientation);

    QBoxLayout* box() const { return box_; }

protected:
    QBoxLayout* box_;
};

// Just a groupbox
class BasicPanel : public QGroupBox, protected RefHub {
private:
    using super = QGroupBox;
public:
    BasicPanel(TheHub&);
    BasicPanel(TheHub&, rcstr title);

    void setHorizontalStretch(int);
    void setVerticalStretch(int);
    void setStretch(int horizontal, int vertical);
};

// A panel with a box layout
class BoxPanel : public BasicPanel {
private:
    using super = BasicPanel;
public:
    BoxPanel(TheHub&, Qt::Orientation);
    BoxPanel(TheHub&, rcstr title, Qt::Orientation);

    QBoxLayout* box() const { return box_; }

protected:
    QBoxLayout* box_;
};

// A panel with grid layout
class GridPanel : public BasicPanel {
private:
    using super = BasicPanel;
public:
    GridPanel(TheHub&);
    GridPanel(TheHub&, rcstr title);

    GridLayout* grid() const { return grid_; }

protected:
    GridLayout* grid_;
};

// A tabbed panel
class Tab : public QWidget {
private:
    using super = QWidget;
public:
    Tab(Qt::Orientation);

    QBoxLayout& box() const { return *box_; }

protected:
    QBoxLayout* box_;
};

class TabsPanel : public QTabWidget, protected RefHub {
private:
    using super = QTabWidget;
public:
    TabsPanel(TheHub&);

    Tab& addTab(rcstr title, Qt::Orientation);
    Tab& tab(uint);
};
}
}
#endif // PANEL_H

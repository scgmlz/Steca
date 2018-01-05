// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/panel.h
//! @brief     Defines classes Tab, TabsPanel
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PANEL_H
#define PANEL_H

#include "typ/str.h"
#include <QTabWidget>

class QBoxLayout;


namespace panel {

//! A tabbed panel
class PTab : public QWidget {  // TODO: better names for panel::Tab and output::Tab
public:
    PTab(Qt::Orientation);
    QBoxLayout& box() const { return *box_; }

protected:
    QBoxLayout* box_;
};


//!
class TabsPanel : public QTabWidget {
public:
    TabsPanel();
    PTab& addTab(rcstr title, Qt::Orientation);
    PTab& tab(uint);
};

} // namespace panel


#endif // PANEL_H

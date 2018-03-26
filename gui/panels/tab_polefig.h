// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_polefig.h
//! @brief     Defines class PolefigTab
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TAB_POLEFIG_H
#define TAB_POLEFIG_H

#include <QWidget>

//! Mainframe tab to plot a polefigure plot, with associated controls.

class PolefigTab : public QWidget {
public:
    PolefigTab();
    void render();
private:
    class PlotPolefig* plot_;
    void showEvent(QShowEvent*) { render(); }
};

#endif // TAB_POLEFIG_H

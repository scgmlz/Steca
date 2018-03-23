// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_polefig.h
//! @brief     Defines class PolefigWidget, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TAB_POLEFIG_H
#define TAB_POLEFIG_H

#include "gui/mainwin.h"

//! Tabular display of fit results and metadata, with associated controls.

class PolefigWidget : public QWidget {
public:
    PolefigWidget();
    void render();
private:
    class PlotPolefig* plot_;
};

#endif // TAB_POLEFIG_H

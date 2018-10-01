//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/popup_diagram.cpp
//! @brief     Implements class PopupDiagram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/popup_diagram.h"
#include "gui/mainwin.h"
#include "gui/view/plot_diagram.h"
#include <QVBoxLayout>

PopupDiagram::PopupDiagram()
    : PopupBaseDialog(gGui, "diagram#", "Diagram")
{
    // initializations
    auto* plot = new PlotDiagram(300,300); // TODO store and reuse user setting; freeze aspect ratio

    // layout
    auto* layout = new QVBoxLayout;
    layout->addWidget(plot);
    layout->setStretch(0,1000);
    // TODO (issue#76) add info about used parameters
    setLayout(layout);

    plot->refresh();
    show();
}

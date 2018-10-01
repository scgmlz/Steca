//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/popup_polefig.cpp
//! @brief     Implements class PopupPolefig
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/popup_polefig.h"
#include "gui/mainwin.h"
#include "gui/view/plot_polefig.h"
#include <QLayout>

PopupPolefig::PopupPolefig()
    : PopupBaseDialog(gGui, "polefig#", "Polefigure")
{
    auto* plot = new PlotPolefig(false); // the main subframe
    plot->setMinimumSize(300,300); // TODO store and reuse user setting; freeze aspect ratio

    auto* layout = new QVBoxLayout;
    layout->addWidget(plot);
    layout->setStretch(0,1000);
    // TODO (issue#76) add info about used parameters
    setLayout(layout);


    show();
}

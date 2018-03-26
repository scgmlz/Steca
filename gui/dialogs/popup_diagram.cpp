// ************************************************************************** //
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
// ************************************************************************** //

#include "popup_diagram.h"
#include "core/session.h"
#include "gui/plot/plot_diagram.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include <QVBoxLayout>

PopupDiagram::PopupDiagram()
    : CModelessDialog(gGui, "polefig#")
{
    setWindowTitle("Steca " + name());

    // initializations
    plot_ = new PlotDiagram; // the main subframe
    plot_->setMinimumSize(300,300); // TODO store and reuse user setting; freeze aspect ratio

    // layout
    auto* layout = new QVBoxLayout;
    layout->addWidget(plot_);
    layout->setStretch(0,1000);
    // TODO add info about used parameters
    setLayout(layout);

    plot_->refresh();
    show();
}

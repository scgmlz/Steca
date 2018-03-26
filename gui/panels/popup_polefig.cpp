// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/popup_polefig.cpp
//! @brief     Implements class PopupPolefig
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "popup_polefig.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/plot/plot_polefig.h"
#include "gui/mainwin.h"


// ************************************************************************** //
//  class PopupPolefig
// ************************************************************************** //

PopupPolefig::PopupPolefig()
    : CModelessDialog(gGui, "polefig#")
{
    setWindowTitle("Steca " + name());

    // initializations
    plot_ = new PlotPolefig; // the main subframe
    plot_->setMinimumSize(300,300); // TODO store and reuse user setting; freeze aspect ratio

    // layout
    auto* layout = new QVBoxLayout;
    layout->addWidget(plot_);
    layout->setStretch(0,1000);
    // TODO add info about used parameters
    setLayout(layout);

    plot_->set(gSession->peakInfos(), false); // what shall we TODO with cbFlat_->isChecked()) ?
    show();
}

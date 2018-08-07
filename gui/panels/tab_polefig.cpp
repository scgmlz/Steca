//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_polefig.cpp
//! @brief     Implements class PolefigTab, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "tab_polefig.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "gui/view/plot_polefig.h"


PolefigTab::PolefigTab()
    : QcrWidget("polefig")
{
    // initializations
    auto plot = new PlotPolefig; // the main subframe

    // layout
    auto* buttonBox = new QHBoxLayout;
    buttonBox->addStretch(1);
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->spawnPolefig});
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->exportPolefig});

    auto* controls = new QVBoxLayout;
    controls->addWidget(gGui->state->polefigShowGridPts);
    controls->addStretch(1); // ---
    controls->addLayout(buttonBox);

    auto* layout = new QHBoxLayout;
    layout->addWidget(plot);
    layout->addLayout(controls);
    layout->setStretch(0,1000);
    setLayout(layout);

    setRemake ([=](){plot->refresh();});
}

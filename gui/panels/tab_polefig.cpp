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

#include "gui/panels/tab_polefig.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"
#include "gui/view/plot_polefig.h"

PolefigTab::PolefigTab()
{
    auto* plot = new PlotPolefig(true);

    auto* buttonBox = new QHBoxLayout;
    buttonBox->addStretch(1);
    buttonBox->addWidget(new QcrIconTriggerButton{&gGui->triggers->spawnPolefig});
    buttonBox->addWidget(new QcrIconTriggerButton{&gGui->triggers->exportPolefig});

    auto* controls = new QVBoxLayout;
    controls->addWidget(new QcrCheckBox{"gridPts", "grid points", &plot->flat});
    controls->addStretch(1); // ---
    controls->addLayout(buttonBox);

    auto* layout = new QHBoxLayout;
    layout->addWidget(plot);
    layout->addLayout(controls);
    layout->setStretch(0,1000);
    setLayout(layout);
}

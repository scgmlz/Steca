//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_diagram.cpp
//! @brief     Implements class DiagramTab, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "tab_diagram.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "gui/view/plot_diagram.h"

#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

//  ***********************************************************************************************
//! @class DiagramTab

DiagramTab::DiagramTab()
{
    auto* plot = new PlotDiagram; // the main subframe

    auto* selectorBox = new QGridLayout;
    selectorBox->addWidget(new QLabel("x"), 0, 0);
    selectorBox->addWidget(gGui->state->diagramX, 0, 1);
    selectorBox->addWidget(new QLabel("y"), 1, 0);
    selectorBox->addWidget(gGui->state->diagramY, 1, 1);

    auto* buttonBox = new QHBoxLayout;
    buttonBox->addStretch(1);
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->spawnDiagram});
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->exportDiagram});

    auto* controls = new QVBoxLayout;
    controls->addLayout(selectorBox);
    controls->addStretch(1); // ---
    controls->addLayout(buttonBox);

    auto* layout = new QHBoxLayout;
    layout->addWidget(plot);
    layout->addLayout(controls);
    layout->setStretch(0,1000);
    setLayout(layout);

    setRemake([=](){plot->refresh();});
}

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
#include "gui/view/plot_diagram.h"
#include "gui/state.h"

#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

//  ***********************************************************************************************
//! @class DiagramTab

DiagramTab::DiagramTab()
{
    // initializations
    plot_ = new PlotDiagram; // the main subframe

    // internal connections
    connect(gGui->state->diagramX, _SLOT_(QComboBox,currentIndexChanged,int), [this]() {
            render(); });
    connect(gGui->state->diagramY, _SLOT_(QComboBox,currentIndexChanged,int), [this]() {
            render(); });

    // layout
    auto* selectorBox = new QGridLayout;
    selectorBox->addWidget(new QLabel("x"), 0, 0);
    selectorBox->addWidget(gGui->state->diagramX, 0, 1);
    selectorBox->addWidget(new QLabel("y"), 1, 0);
    selectorBox->addWidget(gGui->state->diagramY, 1, 1);

    auto* buttonBox = new QHBoxLayout;
    buttonBox->addStretch(1);
    buttonBox->addWidget(new QcrIconButton {&gGui->triggers->spawnDiagram});
    buttonBox->addWidget(new QcrIconButton {&gGui->triggers->exportDiagram});

    auto* controls = new QVBoxLayout;
    controls->addLayout(selectorBox);
    controls->addStretch(1); // ---
    controls->addLayout(buttonBox);

    auto* layout = new QHBoxLayout;
    layout->addWidget(plot_);
    layout->addLayout(controls);
    layout->setStretch(0,1000);
    setLayout(layout);
}

void DiagramTab::render()
{
    if (!isVisible())
        return;
    plot_->refresh();
}

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

#include "gui/panels/tab_diagram.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"
#include "gui/view/plot_diagram.h"

//  ***********************************************************************************************
//! @class DiagramTab

DiagramTab::DiagramTab()
{
    auto* plot = new PlotDiagram; // the main subframe

    // TODO cache tag list
    auto* comboX =
        new QcrComboBox {"diagramCoordX", &gSession->params.diagramX, &PeakInfo::metaTags};
    auto* comboY =
        new QcrComboBox {"diagramCoordY", &gSession->params.diagramY, &PeakInfo::metaTags};

    auto* selectorBox = new QGridLayout;
    selectorBox->addWidget(new QLabel("y"), 0, 0);
    selectorBox->addWidget(comboY,          0, 1);
    selectorBox->addWidget(new QLabel("x"), 1, 0);
    selectorBox->addWidget(comboX,          1, 1);

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

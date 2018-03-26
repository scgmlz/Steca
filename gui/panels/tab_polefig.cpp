// ************************************************************************** //
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
// ************************************************************************** //

#include "tab_polefig.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/plot/plot_polefig.h"
#include "gui/mainwin.h"
#include "gui/state.h"


PolefigTab::PolefigTab()
{
    // initializations
    plot_ = new PlotPolefig; // the main subframe

    // internal connections
    connect(gGui->state->polefigShowGridPts, &QCheckBox::toggled, [this]() { render(); });

    // inbound connection
    connect(gSession, &Session::sigRawFits, [this]() { render(); });

    // layout
    auto* buttonBox = new QHBoxLayout;
    buttonBox->addStretch(1);
    buttonBox->addWidget(new XIconButton {&gGui->triggers->spawnPolefig});
    buttonBox->addWidget(new XIconButton {&gGui->triggers->exportPolefig});

    auto* controls = new QVBoxLayout;
    controls->addWidget(gGui->state->polefigShowGridPts);
    controls->addStretch(1); // ---
    controls->addLayout(buttonBox);

    auto* layout = new QHBoxLayout;
    layout->addWidget(plot_);
    layout->addLayout(controls);
    layout->setStretch(0,1000);
    setLayout(layout);
}

void PolefigTab::render()
{
    if (!isVisible())
        return;
    plot_->set(gSession->peakInfos(), gGui->state->polefigShowGridPts->checkState());
}

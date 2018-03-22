// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_polefig.cpp
//! @brief     Implements class PolefigWidget, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "tab_polefig.h"
#include "core/session.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/panels/plot_polefig.h"
#include <iostream>


// ************************************************************************** //
//  class PolefigWidget
// ************************************************************************** //

PolefigWidget::PolefigWidget()
{
    // initializations
    plot_ = new PlotPolefig; // the main subframe

    // internal connections

    // inbound connection
    connect(gSession, &Session::sigPeaks, [this]() {
            if (isVisible())
                render(); });

    // layout
    auto* buttonBox = new QHBoxLayout;
    buttonBox->addStretch(1);
    buttonBox->addWidget(new XIconButton {&gGui->triggers->spawnPolefig});
    buttonBox->addWidget(new XIconButton {&gGui->triggers->exportPolefig});

    auto* controls = new QVBoxLayout;
    //controls->addLayout(selectXY_);
    controls->addStretch(1); // ---
    controls->addLayout(buttonBox);

    auto* layout = new QHBoxLayout;
    layout->addWidget(plot_);
    layout->addLayout(controls);
    layout->setStretch(0,1000);
    setLayout(layout);
}

void PolefigWidget::render()
{
    int iRefl = gSession->peaks().selectedIndex();
    TakesLongTime __;

    // TODO rm DUPLICATE from TableWidget:

    QVector<PeakInfos> calcPoints_;
    int reflCount = gSession->peaks().count();
    if (!reflCount)
        return;
    Progress progress(reflCount, &gGui->progressBar);

    for_i (reflCount)
        calcPoints_.append(
            gSession->makePeakInfos(
                gSession->peaks().at(i),
                gSession->gammaSelection().numSlices(),
                gSession->gammaSelection().range(),
                &progress));

    if (calcPoints_.isEmpty())
        THROW("calcPoints is empty");
    plot_->set(calcPoints_.at(iRefl));
    // TODO restore interpolation
    //if (!interpPoints_.isEmpty() && !calcPoints_.isEmpty())
    //    plot_->set((interpolated ? interpPoints_ : calcPoints_).at(reflIndex));
}

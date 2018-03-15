// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_setup.cpp
//! @brief     Implements class SubframeSetup
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "subframe_setup.h"
#include "core/session.h"
#include "gui/panels/controls_baseline.h"
#include "gui/panels/controls_detector.h"
#include "gui/panels/controls_peakfits.h"
#include "gui/mainwin.h"

SubframeSetup::SubframeSetup()
{
    setTabPosition(QTabWidget::North);

    addTab(new ControlsDetector(), "Detector");
    addTab(new ControlsBaseline(), "Baseline");
    addTab(new ControlsPeakfits(), "Peakfits");

    connect(this, &SubframeSetup::currentChanged, [this](int index) {
            gGui->baselineEditable = (index==1);
            gGui->peaksEditable    = (index==2);
            emit gSession->sigDiffractogram();
        });

    connect(gSession, &Session::sigFiles, this, &SubframeSetup::updateTabsAvailability);

    updateTabsAvailability();
}

void SubframeSetup::updateTabsAvailability()
{
    if (gSession->dataset().countFiles()) {
        setTabEnabled(1, true);
        setTabEnabled(2, true);
    } else {
        setTabEnabled(1, false);
        setTabEnabled(2, false);
        setCurrentIndex(0);
    }
}

//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "subframe_setup.h"
#include "core/session.h"
#include "gui/panels/controls_baseline.h"
#include "gui/panels/controls_detector.h"
#include "gui/panels/controls_interpolation.h"
#include "gui/panels/controls_peakfits.h"

SubframeSetup::SubframeSetup()
    : QcrTabWidget {"setupTabs"}
{
    setTabPosition(QTabWidget::North);

    addTab(new ControlsDetector(), "Detector");
    addTab(new ControlsBaseline(), "Baseline");
    addTab(new ControlsPeakfits(), "Peakfits");
    addTab(new ControlsInterpolation(), "Interpol");

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
        programaticallySetValue(0);
    }
}

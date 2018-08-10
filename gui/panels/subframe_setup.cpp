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
    setMinimumSize(270,320);

    addTab(new ControlsDetector(),     "Detector"); idxDetector = 0;
    addTab(new ControlsBaseline(),     "Baseline"); idxBaseline = 1;
    addTab(new ControlsPeakfits(),     "Peakfits"); idxPeakfits = 2;
    addTab(new ControlsInterpolation(),"Interpol"); idxInterpol = 3;

    setRemake( [=]() {
            setTabEnabled(idxBaseline, gSession->dataset.countFiles());
            setTabEnabled(idxPeakfits, gSession->dataset.countFiles());
            if (!currentWidget()->isEnabled())
                programaticallySetValue(0);
        } );
}

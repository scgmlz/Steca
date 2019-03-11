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

#include "gui/panels/subframe_setup.h"
#include "core/session.h"
#include "gui/panels/controls_baseline.h"
#include "gui/panels/controls_detector.h"
#include "gui/panels/controls_interpolation.h"
#include "gui/panels/controls_peakfits.h"

SubframeSetup::SubframeSetup()
    : QcrTabWidget{"setupTabs"}
{
    setTabPosition(QTabWidget::North);
    setMinimumSize(270,320);

    addTab(new ControlsDetector,     "Detector"); // 0
    addTab(new ControlsBaseline,     "Baseline"); // 1
    addTab(new ControlsPeakfits,     "Peakfits"); // 2
    addTab(new ControlsInterpolation,"Interpol"); // 3

    setHook([=](const int val){
            ASSERT(val==this->currentIndex());
            switch (val) {
            case 1:   gSession->params.editableRange = EditableRange::BASELINE; break;
            case 2:   gSession->params.editableRange = EditableRange::PEAKS;    break;
            default:  gSession->params.editableRange = EditableRange::NONE;
            }
        });
    setRemake([=](){
            setTabEnabled(1, gSession->dataset.countFiles());
            setTabEnabled(2, gSession->dataset.countFiles());
            if (!currentWidget()->isEnabled())
                setCellValue(0);
        });
}

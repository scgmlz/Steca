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
    : QcrTabWidget {"setupTabs"}
{
    setTabPosition(QTabWidget::North);
    setMinimumSize(270,320);

    addTab(new ControlsDetector(),     "Detector"); //const int idxDetector = 0;
    addTab(new ControlsBaseline(),     "Baseline"); const int idxBaseline = 1;
    addTab(new ControlsPeakfits(),     "Peakfits"); const int idxPeakfits = 2;
    addTab(new ControlsInterpolation(),"Interpol"); //const int idxInterpol = 3;

    setHook([=](int val){
            ASSERT(val==this->currentIndex());
            switch (val) {
            case idxBaseline: gSession->params.editableRange = EditableRange::BASELINE; break;
            case idxPeakfits: gSession->params.editableRange = EditableRange::PEAKS; break;
            default:          gSession->params.editableRange = EditableRange::NONE;
            }
        });
    setRemake([=](){
            setTabEnabled(idxBaseline, gSession->dataset.countFiles());
            setTabEnabled(idxPeakfits, gSession->dataset.countFiles());
            if (!currentWidget()->isEnabled())
                programaticallySetValue(0);
        });
}

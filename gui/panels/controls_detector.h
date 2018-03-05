// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_detector.h
//! @brief     Defines class ControlsDetector
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CONTROLS_DETECTOR_H
#define CONTROLS_DETECTOR_H

#include "gui/base/controls.h"
#include "gui/actions/toggles.h"

//! Row of controls to combine Measurement|s into Cluster|s.

class ExperimentControls : public QWidget {
public:
    ExperimentControls();
private:
    CSpinBox combineMeasurements_ {"combineMeasurements", 4, false, 1, INT_MAX,
            "Combine this number of measurements into one group"};
    CToggle dropIncompleteAction_ {"dropIncomplete",
            "Drop measurement groups that do not have the full number of members",
            false, ":/icon/dropIncomplete" };
    XIconButton dropIncompleteButton_ { &dropIncompleteAction_ };
};

//! A widget with controls to view and change the detector geometry.

class ControlsDetector : public QWidget {
public:
    ControlsDetector();
private:
    void toSession();
    void fromSession();

    CDoubleSpinBox detDistance_, detPixelSize_;
    CSpinBox beamOffsetI_, beamOffsetJ_;
    CSpinBox cutLeft_, cutTop_, cutRight_, cutBottom_;
    ExperimentControls experimentControls;
};

#endif // CONTROLS_DETECTOR_H

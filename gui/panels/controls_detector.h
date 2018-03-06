// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_detector.h
//! @brief     Defines classes (Cut|Experiment|Geometry)Controls, ControlsDetector
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CONTROLS_DETECTOR_H
#define CONTROLS_DETECTOR_H

#include "core/data/geometry.h"
#include "gui/mainwin.h"
#include "gui/base/controls.h"
#include "gui/actions/toggles.h"

//! Control widgets that govern the detector cuts.

class CutControls : public QFrame {
public:
    CutControls();
private:
    void fromCore();

    QGridLayout layout_;
    XIconButton link_ {&gGui->toggles->linkCuts};
    CSpinBox cutLeft_ {"cutLeft", 4, false, 0};
    CSpinBox cutTop_ {"cutTop", 4, false, 0};
    CSpinBox cutRight_ {"cutRight", 4, false, 0};
    CSpinBox cutBottom_ {"cutBottom", 4, false, 0};
};

//! Control widgets that govern the combination of Measurement|s into Cluster|s.

class ExperimentControls : public QWidget {
public:
    ExperimentControls();
private:
    void fromCore();

    QHBoxLayout layout_;
    CSpinBox combineMeasurements_ {"combineMeasurements", 4, false, 1, INT_MAX,
            "Combine this number of measurements into one group"};
    CToggle dropIncompleteAction_ {"dropIncomplete",
            "Drop measurement groups that do not have the full number of members",
            false, ":/icon/dropIncomplete" };
    XIconButton dropIncompleteButton_ { &dropIncompleteAction_ };
};

//! Control widgets that govern the detector geometry.

class GeometryControls : public QWidget {
public:
    GeometryControls();
private:
    void fromCore();

    QVBoxLayout vbox_;
    QGridLayout mmGrid_;
    QHBoxLayout trafoLayout_;
    QHBoxLayout offsetLayout_;

    CDoubleSpinBox detDistance_ {"detDistance", 6};
    CDoubleSpinBox detPixelSize_ {"detPixelSize", 6};
    CSpinBox beamOffsetI_ {"beamOffsetI", 6, true};
    CSpinBox beamOffsetJ_ {"beamOffsetJ", 6, true};
};

//! Aggregated control widgets that govern the detector setup.

class ControlsDetector : public QWidget {
public:
    ControlsDetector();
private:
    QVBoxLayout vbox_;
    CutControls cutControls_;
    ExperimentControls experimentControls_;
    GeometryControls geometryControls_;
};

#endif // CONTROLS_DETECTOR_H

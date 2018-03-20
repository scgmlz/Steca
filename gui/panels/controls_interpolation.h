// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_interpolation.h
//! @brief     Defines class SubframeSetup
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CONTROLS_INTERPOLATION_H
#define CONTROLS_INTERPOLATION_H

#include "gui/base/controls.h"

//! A widget with controls to change the interpolation fitting.

class ControlsInterpolation : public QWidget {
public:
    ControlsInterpolation();
private:
    void fromCore();
    CDoubleSpinBox stepAlpha_ {"stepAlpha", 6, 1., 30.};
    CDoubleSpinBox stepBeta_ {"stepBeta", 6, 1., 30.};
    CDoubleSpinBox idwRadius_ {"idwRadius", 6, 0., 90.};
    CDoubleSpinBox avgAlphaMax_ {"avgAlphaMax", 6, 0., 90.};
    CDoubleSpinBox avgRadius_ {"avgRadius", 6, 0., 90.};
    CSpinBox       threshold_ {"threshold", 6, 0, 100};
};

#endif // CONTROLS_INTERPOLATION_H

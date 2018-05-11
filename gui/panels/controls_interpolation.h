//  ***********************************************************************************************
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
//  ***********************************************************************************************

#ifndef CONTROLS_INTERPOLATION_H
#define CONTROLS_INTERPOLATION_H

#include "qcr/widgets/controls.h"

//! A widget with controls to change the interpolation fitting.

class ControlsInterpolation : public QWidget {
public:
    ControlsInterpolation();
private:
    void fromCore();
    QcrCheckBox*      doInterpol_ ;
    QcrDoubleSpinBox* stepAlpha_  ;
    QcrDoubleSpinBox* stepBeta_   ;
    QcrDoubleSpinBox* idwRadius_  ;
    QcrDoubleSpinBox* avgAlphaMax_;
    QcrDoubleSpinBox* avgRadius_  ;
    QcrSpinBox*       threshold_  ;
};

#endif // CONTROLS_INTERPOLATION_H

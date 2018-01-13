// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_setup.h
//! @brief     Defines class SubframeSetup
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SUBFRAME_SETUP_H
#define SUBFRAME_SETUP_H

#include "gui/base/new_q.h"
#include <QTabWidget>

//! Part of the main window that contains geometry, background, and image controls.

class SubframeSetup : public QTabWidget {
public:
    SubframeSetup();

private:
    // image geometry tab
    QDoubleSpinBox *detDistance_, *detPixelSize_;
    QSpinBox *beamOffsetI_, *beamOffsetJ_;
    QSpinBox *cutLeft_, *cutTop_, *cutRight_, *cutBottom_;

    // background fit tab
    QSpinBox* spinDegree_;

    // peak fits tab
    class ReflectionView* reflectionView_;
    QComboBox* comboReflType_;
    QDoubleSpinBox *spinRangeMin_, *spinRangeMax_;
    QDoubleSpinBox *spinGuessPeakX_, *spinGuessPeakY_, *spinGuessFWHM_;
    QLineEdit *readFitPeakX_, *readFitPeakY_, *readFitFWHM_;

    bool silentSpin_ = false;

    void setToHub();
    void setFromHub();
};

#endif // SUBFRAME_SETUP_H

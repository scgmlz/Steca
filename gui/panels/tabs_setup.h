// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tabs_setup.h
//! @brief     Defines class ViewSetup
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TABS_SETUP_H
#define TABS_SETUP_H

#include <QTabWidget>

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;

//! Part of the main window that contains geometry, background, and image settings.
class ViewSetup : public QTabWidget {
public:
    ViewSetup();

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

#endif // TABS_SETUP_H

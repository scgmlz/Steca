// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/dialog_panels.h
//! @brief     Defines PanelPeak, PanelGammaSlices. and several other panel classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DIALOG_PANELS_H
#define DIALOG_PANELS_H

#include "core/typ/range.h"
#include "gui/cfg/settings.h"
#include "gui/base/controls.h"
#include <QGroupBox>

//! A panel with grid layout

class GridPanel : public QGroupBox {
public:
    GridPanel(const QString& title="");

    void setHorizontalStretch(int);
    void setVerticalStretch(int);
    void setStretch(int horizontal, int vertical);

    QGridLayout grid_; // needed by TabSave, TabDiagramsSave, TabD'gramSave, and others; therefore public
};


/* Note that some data members are public, to simplify the code. Be careful. */

//! A combo box to select one of the defined Bragg peaks.

class PanelPeak : public GridPanel {
public:
    PanelPeak();
    CComboBox cbRefl;
};

//! Controls to define the gamma slicing.

class PanelGammaSlices : public GridPanel {
public:
    PanelGammaSlices();
    ~PanelGammaSlices();
    CSpinBox numSlices {"numSlices#", 4, false, 0};
    CDoubleSpinBox stepGamma {"stepGamma#", 6, 0.0};
    void updateValues();
private:
    Range rgeGma_;
    XSettings settings_;
};

//! Displays the current gamma range.

class PanelGammaRange : public GridPanel {
public:
    PanelGammaRange();
    ~PanelGammaRange();
    CCheckBox cbLimitGamma {"cbLimitGamma#", "limit"};
    CDoubleSpinBox minGamma {"minGamma#", 6, -180., 180.};
    CDoubleSpinBox maxGamma {"maxGamma#", 6, -180., 180.};
    void updateValues();
private:
    Range rgeGma_;
    XSettings settings_;
};

//! Control to choose calculation vs interpolation, in PoleFiguresFrame.

class PanelPoints : public GridPanel {
public:
    PanelPoints();
    ~PanelPoints();
    CRadioButton rbCalc {"rbCalc#", "calculated"};
    CRadioButton rbInterp {"rbInterp#", "interpolated"};
private:
    XSettings settings_;
};

//! Controls to choose tune the interpolation, in PoleFiguresFrame.

class PanelInterpolation : public GridPanel {
public:
    PanelInterpolation();
    ~PanelInterpolation();
    CDoubleSpinBox stepAlpha {"stepAlpha#", 6, 1., 30.};
    CDoubleSpinBox stepBeta {"stepBeta#", 6, 1., 30.};
    CDoubleSpinBox idwRadius {"idwRadius#", 6, 0., 90.};
    CDoubleSpinBox avgAlphaMax {"avgAlphaMax#", 6, 0., 90.};
    CDoubleSpinBox avgRadius {"avgRadius#", 6, 0., 90.};
    CSpinBox avgThreshold {"avgThreshold#", 6, 0, 100};
private:
    XSettings settings_;
};

//! Controls to choose metadata?

class PanelDiagram : public GridPanel {
public:
    PanelDiagram();
    CComboBox xAxis {"xAxis#"};
    CComboBox yAxis {"yAxis#"};
};

#endif // DIALOG_PANELS_H

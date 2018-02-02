// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/dialog_panels.h
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
#include "gui/base/new_q.h"
#include <QGroupBox>

//! A panel with grid layout

class GridPanel : public QGroupBox {
public:
    GridPanel(rcstr title="");

    void setHorizontalStretch(int);
    void setVerticalStretch(int);
    void setStretch(int horizontal, int vertical);

    QGridLayout* grid() const { return grid_; }

protected:
    QGridLayout* grid_;
};


/* Note that some data members are public, to simplify the code. Be careful. */

//! A combo box to select one of the defined Bragg peaks.

class PanelPeak : public GridPanel {
public:
    PanelPeak();
    QComboBox* cbRefl;
};

//! Controls to define the gamma slicing.

class PanelGammaSlices : public GridPanel {
public:
    PanelGammaSlices();
    ~PanelGammaSlices();
    QSpinBox* numSlices;
    QDoubleSpinBox* stepGamma;
    void updateValues();
private:
    Range rgeGma_;
    Settings settings_;
};

//! Displays the current gamma range.

class PanelGammaRange : public GridPanel {
public:
    PanelGammaRange();
    ~PanelGammaRange();
    QCheckBox* cbLimitGamma;
    QDoubleSpinBox *minGamma, *maxGamma;
    void updateValues();
private:
    Range rgeGma_;
    Settings settings_;
};

//! Control to choose calculation vs interpolation, in PoleFiguresFrame.

class PanelPoints : public GridPanel {
public:
    PanelPoints();
    ~PanelPoints();
    QRadioButton *rbCalc, *rbInterp;
private:
    Settings settings_;
};

//! Controls to choose tune the interpolation, in PoleFiguresFrame.

class PanelInterpolation : public GridPanel {
public:
    PanelInterpolation();
    ~PanelInterpolation();
    QDoubleSpinBox *stepAlpha, *stepBeta, *idwRadius;
    QDoubleSpinBox *avgAlphaMax, *avgRadius;
    QSpinBox* avgThreshold;
private:
    Settings settings_;
};

//! Controls to choose metadata?

class PanelDiagram : public GridPanel {
public:
    PanelDiagram();
    QComboBox *xAxis, *yAxis;
};

#endif // DIALOG_PANELS_H

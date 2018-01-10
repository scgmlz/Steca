// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/dialog_panels.h
//! @brief     Defines PanelReflection, PanelGammaSlices. and several other panel classes
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
#include "gui/widgets/new_q.h"
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

class PanelReflection : public GridPanel {
public:
    PanelReflection();
    QComboBox* cbRefl;
};


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


class PanelPoints : public GridPanel {
public:
    PanelPoints();
    QRadioButton *rbCalc, *rbInterp;
};


class PanelInterpolation : public GridPanel {
public:
    PanelInterpolation();
    QDoubleSpinBox *stepAlpha, *stepBeta, *idwRadius;
    QDoubleSpinBox *avgAlphaMax, *avgRadius;
    QSpinBox* avgThreshold;
};


class PanelDiagram : public GridPanel {
public:
    PanelDiagram();
    QComboBox *xAxis, *yAxis;
};


class PanelFitError : public GridPanel {
public:
    PanelFitError();
};

#endif // DIALOG_PANELS_H

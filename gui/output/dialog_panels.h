// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/dialog_panels.h
//! @brief     Defines PanelReflection, PanelGammaSlices. and several other panel classes
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DIALOG_PANELS_H
#define DIALOG_PANELS_H

#include "panels/panel.h"
#include "typ/range.h"
#include <QCheckBox>

namespace gui {

namespace output {

/* Note that some data members are public, to simplify the code. Be careful. */

class PanelReflection : public panel::GridPanel {
public:
    PanelReflection();
    QComboBox* cbRefl;
};


class PanelGammaSlices : public panel::GridPanel {
public:
    PanelGammaSlices();
    QSpinBox* numSlices;
    QDoubleSpinBox* stepGamma;
    void updateValues();
private:
    typ::Range rgeGma_;
};


class PanelGammaRange : public panel::GridPanel {
public:
    PanelGammaRange();
    QCheckBox* cbLimitGamma;
    QDoubleSpinBox *minGamma, *maxGamma;
    void updateValues();
private:
    typ::Range rgeGma_;
};


class PanelPoints : public panel::GridPanel {
public:
    PanelPoints();
    QRadioButton *rbCalc, *rbInterp;
};


class PanelInterpolation : public panel::GridPanel {
public:
    PanelInterpolation();
    QDoubleSpinBox *stepAlpha, *stepBeta, *idwRadius;
    QDoubleSpinBox *avgAlphaMax, *avgRadius;
    QSpinBox* avgThreshold;
};


class PanelDiagram : public panel::GridPanel {
public:
    PanelDiagram();
    QComboBox *xAxis, *yAxis;
};


class PanelFitError : public panel::GridPanel {
public:
    PanelFitError();
};

} // namespace output
} // namespace gui

#endif // DIALOG_PANELS_H

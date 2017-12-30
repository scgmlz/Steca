// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/output_dialogs.h
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

#include "calc/calc_reflection_info.h"
#include "panels/panel.h"

namespace gui {

namespace output {

/* Note that some data members are public, to simplify the code. Be careful. */

class PanelReflection : public panel::GridPanel {
public:
    PanelReflection(TheHub&);
    QComboBox* cbRefl;
};


class PanelGammaSlices : public panel::GridPanel {
public:
    PanelGammaSlices(TheHub&);
    QSpinBox* numSlices;
    QDoubleSpinBox* stepGamma;
    void updateValues();
private:
    typ::Range rgeGma_;
};


class PanelGammaRange : public panel::GridPanel {
public:
    PanelGammaRange(TheHub&);
    QCheckBox* cbLimitGamma;
    QDoubleSpinBox *minGamma, *maxGamma;
    void updateValues();
private:
    typ::Range rgeGma_;
};


class PanelPoints : public panel::GridPanel {
public:
    PanelPoints(TheHub&);
    QRadioButton *rbCalc, *rbInterp;
};


class PanelInterpolation : public panel::GridPanel {
public:
    PanelInterpolation(TheHub&);
    QDoubleSpinBox *stepAlpha, *stepBeta, *idwRadius;
    QDoubleSpinBox *avgAlphaMax, *avgRadius;
    QSpinBox* avgThreshold;
};


class PanelDiagram : public panel::GridPanel {
public:
    PanelDiagram(TheHub&);
    QComboBox *xAxis, *yAxis;
};


class PanelFitError : public panel::GridPanel {
public:
    PanelFitError(TheHub&);
};

} // namespace output
} // namespace gui

#endif // DIALOG_PANELS_H

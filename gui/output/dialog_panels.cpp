// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/dialog_panels.cpp
//! @brief     Implements PanelPeak, PanelGammaSlices. and several other panel classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/output/dialog_panels.h"
#include "core/session.h"
#include "gui/thehub.h"

// ************************************************************************** //
//  class GridPanel
// ************************************************************************** //

void GridPanel::setHorizontalStretch(int stretch) {
    QSizePolicy sp = sizePolicy();
    sp.setHorizontalStretch(stretch);
    setSizePolicy(sp);
}

void GridPanel::setVerticalStretch(int stretch) {
    QSizePolicy sp = sizePolicy();
    sp.setVerticalStretch(stretch);
    setSizePolicy(sp);
}

void GridPanel::setStretch(int horizontal, int vertical) {
    QSizePolicy sp = sizePolicy();
    sp.setHorizontalStretch(horizontal);
    sp.setVerticalStretch(vertical);
    setSizePolicy(sp);
}

GridPanel::GridPanel(rcstr title) : QGroupBox(title) {
    setLayout((grid_ = newQ::GridLayout()));
}

// ************************************************************************** //
//  final panel classes
// ************************************************************************** //

PanelPeak::PanelPeak() : GridPanel("Peak") {
    QGridLayout* g = grid();
    cbRefl = newQ::ComboBox("outRefl", gSession->peaks().names());
    g->addWidget(cbRefl);
    g->setRowStretch(g->rowCount(), 1);
}


PanelGammaSlices::PanelGammaSlices() : GridPanel("Gamma slices"), settings_("gamma_slices") {
    QGridLayout* g = grid();

    g->addWidget(newQ::Label("count"), 0, 0);
    g->addWidget((numSlices = newQ::SpinBox("numSlices", 4, false, 0)), 0, 1);
    numSlices->setValue(settings_.readInt("num_slices", 0));

    g->addWidget(newQ::Label("degrees"), 1, 0);
    g->addWidget((stepGamma = newQ::DoubleSpinBox("stepGamma", 6, 0.0)), 1, 1);
    stepGamma->setReadOnly(true);

    g->setRowStretch(g->rowCount(), 1);

    rgeGma_ = gSession->experiment().rgeGma();

    connect(numSlices, _SLOT_(QSpinBox, valueChanged, int), [this]() { updateValues(); });
}

PanelGammaSlices::~PanelGammaSlices() {
    settings_.saveInt("num_slices", numSlices->value());
}

void PanelGammaSlices::updateValues() {
    int nSlices = numSlices->value();
    if (nSlices > 0)
        stepGamma->setValue(rgeGma_.width() / nSlices);
    else
        stepGamma->clear();
}


PanelGammaRange::PanelGammaRange() : GridPanel("Gamma range"), settings_("gamma_range") {
    QGridLayout* g = grid();

    g->addWidget((cbLimitGamma = newQ::CheckBox("cbLimitGamma", "limit")), 0, 0, 1, 2);
    cbLimitGamma->setChecked(settings_.readBool("limit", false));

    g->addWidget(newQ::Label("min"), 1, 0);
    g->addWidget((minGamma = newQ::DoubleSpinBox("minGamma", 6, -180., 180.)), 1, 1);

    g->addWidget(newQ::Label("max"), 2, 0);
    g->addWidget((maxGamma = newQ::DoubleSpinBox("maxGamma", 6, -180., 180.)), 2, 1);

    g->setRowStretch(g->rowCount(), 1);

    rgeGma_ = gSession->experiment().rgeGma();

    minGamma->setValue(rgeGma_.min);
    maxGamma->setValue(rgeGma_.max);

    connect(cbLimitGamma, &QCheckBox::toggled, [this]() { updateValues(); });
}

PanelGammaRange::~PanelGammaRange() {
    settings_.saveBool("limit", cbLimitGamma->isChecked());

}

// TODO when min/maxGamma updated -> reflect that in PanelGammaSlices

void PanelGammaRange::updateValues() {
    bool on = cbLimitGamma->isChecked();
    minGamma->setEnabled(on);
    maxGamma->setEnabled(on);
}


PanelPoints::PanelPoints() : GridPanel("Points"), settings_("polediagram_points") {
    QGridLayout* g = grid();
    g->addWidget((rbCalc = newQ::RadioButton("rbCalc", "calculated")), 0, 0);
    g->addWidget((rbInterp = newQ::RadioButton("rbInterp", "interpolated")), 1, 0);
    (settings_.readBool("interpolated", false) ? rbInterp : rbCalc)->setChecked(true);

    g->setRowStretch(g->rowCount(), 1);
}

PanelPoints::~PanelPoints() {
    settings_.saveBool("interpolated", rbInterp->isChecked());
}


PanelInterpolation::PanelInterpolation() : GridPanel("Interpolation") {
    QGridLayout* g = grid();

    g->addWidget(newQ::Label("step α"), 0, 0, Qt::AlignRight);
    g->addWidget((stepAlpha = newQ::DoubleSpinBox("stepAlpha", 6, 1., 30.)), 0, 1);
    g->addWidget(newQ::Label("β"), 1, 0, Qt::AlignRight);
    g->addWidget((stepBeta = newQ::DoubleSpinBox("stepBeta", 6, 1., 30.)), 1, 1);
    g->addWidget(newQ::Label("idw radius"), 2, 0, Qt::AlignRight);
    g->addWidget((idwRadius = newQ::DoubleSpinBox("idwRadius", 6, 0., 90.)), 2, 1);

    g->addWidget(newQ::Label("avg. α max"), 0, 2, Qt::AlignRight);
    g->addWidget((avgAlphaMax = newQ::DoubleSpinBox("avgAlphaMax", 6, 0., 90.)), 0, 3);
    g->addWidget(newQ::Label("radius"), 1, 2, Qt::AlignRight);
    g->addWidget((avgRadius = newQ::DoubleSpinBox("avgRadius", 6, 0., 90.)), 1, 3);
    g->addWidget(newQ::Label("inclusion %"), 2, 2, Qt::AlignRight);
    g->addWidget((avgThreshold = newQ::SpinBox("avgThreshold", 6, 0, 100)), 2, 3);

    g->setRowStretch(g->rowCount(), 1);

    stepAlpha->setValue(settings_.readReal("step alpha", 5));
    stepBeta->setValue(settings_.readReal("step beta", 5));
    idwRadius->setValue(settings_.readReal("idw radius", 10));

    avgAlphaMax->setValue(settings_.readReal("avg alpha max", 15));
    avgRadius->setValue(settings_.readReal("avg radius", 5));
    avgThreshold->setValue(settings_.readInt("avg threshold", 100));
}

PanelInterpolation::~PanelInterpolation() {
    settings_.saveReal("step alpha", stepAlpha->value());
    settings_.saveReal("step beta", stepBeta->value());
    settings_.saveReal("idw radius", idwRadius->value());
    settings_.saveReal("avg alpha max", avgAlphaMax->value());
    settings_.saveReal("avg radius", avgRadius->value());
    settings_.saveInt("avg threshold", avgThreshold->value());
}


PanelDiagram::PanelDiagram() : GridPanel("Diagram") {
    QStringList tags = PeakInfo::dataTags(false);
    for_i (Metadata::numAttributes(false) - Metadata::numAttributes(true))
        tags.removeLast(); // remove all tags that are not numbers

    QGridLayout* g = grid();
    xAxis = newQ::ComboBox("xAxis", tags);
    yAxis = newQ::ComboBox("yAxis", tags);
    g->addWidget(newQ::Label("x"), 1, 0);
    g->addWidget(xAxis, 1, 1);
    g->addWidget(newQ::Label("y"), 0, 0);
    g->addWidget(yAxis, 0, 1);

    g->setRowStretch(g->rowCount(), 1);
}

// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/dialog_panels.cpp
//! @brief     Implements PanelPeak, PanelGammaSlices. and several other panel classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/dialogs/dialog_panels.h"
#include "core/session.h"
#include "gui/mainwin.h"

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

GridPanel::GridPanel(const QString& title) : QGroupBox(title) {
    setLayout((grid_ = newQ::GridLayout()));
}

// ************************************************************************** //
//  final panel classes
// ************************************************************************** //

PanelPeak::PanelPeak()
    : GridPanel("Peak")
    , cbRefl("outRefl", gSession->peaks().names()) {
    QGridLayout* g = grid();
    g->addWidget(&cbRefl);
    g->setRowStretch(g->rowCount(), 1);
}


PanelGammaSlices::PanelGammaSlices()
    : GridPanel("Gamma slices")
    , numSlices("numSlices", 4, false, 0)
    , stepGamma("stepGamma", 6, 0.0)
    , settings_("gamma_slices") {
    QGridLayout* g = grid();

    g->addWidget(new QLabel("count"), 0, 0);
    numSlices.setValue(settings_.readInt("num_slices", 0));
    g->addWidget(&numSlices, 0, 1);

    g->addWidget(new QLabel("degrees"), 1, 0);
    g->addWidget(&stepGamma, 1, 1);
    stepGamma.setReadOnly(true);

    g->setRowStretch(g->rowCount(), 1);

    rgeGma_ = gSession->experiment().rgeGma();

    connect(&numSlices, _SLOT_(QSpinBox, valueChanged, int), [this]() { updateValues(); });
}

PanelGammaSlices::~PanelGammaSlices() {
    settings_.saveInt("num_slices", numSlices.value());
}

void PanelGammaSlices::updateValues() {
    int nSlices = numSlices.value();
    if (nSlices > 0)
        stepGamma.setValue(rgeGma_.width() / nSlices);
    else
        stepGamma.clear();
}


PanelGammaRange::PanelGammaRange()
    : GridPanel("Gamma range")
    , cbLimitGamma("cbLimitGamma", "limit")
    , minGamma("minGamma", 6, -180., 180.)
    , maxGamma("maxGamma", 6, -180., 180.)
    , settings_("gamma_range")
{
    QGridLayout* g = grid();

    g->addWidget(&cbLimitGamma, 0, 0, 1, 2);
    cbLimitGamma.setChecked(settings_.readBool("limit", false));

    g->addWidget(new QLabel("min"), 1, 0); g->addWidget(&minGamma, 1, 1);
    g->addWidget(new QLabel("max"), 2, 0); g->addWidget(&maxGamma, 2, 1);
    g->setRowStretch(g->rowCount(), 1);

    rgeGma_ = gSession->experiment().rgeGma();

    minGamma.setValue(rgeGma_.min);
    maxGamma.setValue(rgeGma_.max);

    connect(&cbLimitGamma, &QCheckBox::toggled, [this]() { updateValues(); });
}

PanelGammaRange::~PanelGammaRange() {
    settings_.saveBool("limit", cbLimitGamma.isChecked());
}

// TODO when min/maxGamma updated -> reflect that in PanelGammaSlices

void PanelGammaRange::updateValues() {
    bool on = cbLimitGamma.isChecked();
    minGamma.setEnabled(on);
    maxGamma.setEnabled(on);
}


PanelPoints::PanelPoints()
    : GridPanel("Points")
    , rbCalc("rbCalc", "calculated")
    , rbInterp("rbInterp", "interpolated")
    , settings_("polediagram_points")
{
    QGridLayout* g = grid();
    g->addWidget(&rbCalc, 0, 0);
    g->addWidget(&rbInterp, 1, 0);
    (settings_.readBool("interpolated", false) ? rbInterp : rbCalc).setChecked(true);

    g->setRowStretch(g->rowCount(), 1);
}

PanelPoints::~PanelPoints() {
    settings_.saveBool("interpolated", rbInterp.isChecked());
}


PanelInterpolation::PanelInterpolation()
    : GridPanel("Interpolation")
    , stepAlpha("stepAlpha", 6, 1., 30.)
    , stepBeta("stepBeta", 6, 1., 30.)
    , idwRadius("idwRadius", 6, 0., 90.)
    , avgAlphaMax("avgAlphaMax", 6, 0., 90.)
    , avgRadius("avgRadius", 6, 0., 90.)
    , avgThreshold("avgThreshold", 6, 0, 100)
{
    QGridLayout* g = grid();

    g->addWidget(new QLabel("step α"), 0, 0, Qt::AlignRight);
    g->addWidget(&stepAlpha, 0, 1);
    g->addWidget(new QLabel("β"), 1, 0, Qt::AlignRight);
    g->addWidget(&stepBeta, 1, 1);
    g->addWidget(new QLabel("idw radius"), 2, 0, Qt::AlignRight);
    g->addWidget(&idwRadius, 2, 1);
    g->addWidget(new QLabel("avg. α max"), 0, 2, Qt::AlignRight);
    g->addWidget(&avgAlphaMax, 0, 3);
    g->addWidget(new QLabel("radius"), 1, 2, Qt::AlignRight);
    g->addWidget(&avgRadius, 1, 3);
    g->addWidget(new QLabel("inclusion %"), 2, 2, Qt::AlignRight);
    g->addWidget(&avgThreshold, 2, 3);

    g->setRowStretch(g->rowCount(), 1);

    stepAlpha.setValue(settings_.readReal("step alpha", 5));
    stepBeta.setValue(settings_.readReal("step beta", 5));
    idwRadius.setValue(settings_.readReal("idw radius", 10));

    avgAlphaMax.setValue(settings_.readReal("avg alpha max", 15));
    avgRadius.setValue(settings_.readReal("avg radius", 5));
    avgThreshold.setValue(settings_.readInt("avg threshold", 100));
}

PanelInterpolation::~PanelInterpolation() {
    settings_.saveReal("step alpha", stepAlpha.value());
    settings_.saveReal("step beta", stepBeta.value());
    settings_.saveReal("idw radius", idwRadius.value());
    settings_.saveReal("avg alpha max", avgAlphaMax.value());
    settings_.saveReal("avg radius", avgRadius.value());
    settings_.saveInt("avg threshold", avgThreshold.value());
}


PanelDiagram::PanelDiagram()
    : GridPanel("Diagram")
    , xAxis("xAxis")
    , yAxis("yAxis")
{
    QStringList tags = PeakInfo::dataTags(false);
    for_i (Metadata::numAttributes(false) - Metadata::numAttributes(true))
        tags.removeLast(); // remove all tags that are not numbers
    xAxis.addItems(tags);
    yAxis.addItems(tags);

    QGridLayout* g = grid();
    g->addWidget(new QLabel("x"), 1, 0);
    g->addWidget(&xAxis, 1, 1);
    g->addWidget(new QLabel("y"), 0, 0);
    g->addWidget(&yAxis, 0, 1);

    g->setRowStretch(g->rowCount(), 1);
}

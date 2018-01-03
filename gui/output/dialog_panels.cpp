// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/dialog_panels.cpp
//! @brief     Implements PanelReflection, PanelGammaSlices. and several other panel classes
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "dialog_panels.h"
#include "data/metadata.h"
#include "def/idiomatic_for.h"
#include "cfg/gui_cfg.h"
#include "models.h"
#include "session.h"
#include "thehub.h"
#include "widgets/widget_makers.h"

namespace gui {
namespace output {

// ************************************************************************** //
//  class GridPanel
// ************************************************************************** //

void GridPanel::setHorizontalStretch(int stretch) {
    auto sp = sizePolicy();
    sp.setHorizontalStretch(stretch);
    setSizePolicy(sp);
}

void GridPanel::setVerticalStretch(int stretch) {
    auto sp = sizePolicy();
    sp.setVerticalStretch(stretch);
    setSizePolicy(sp);
}

void GridPanel::setStretch(int horizontal, int vertical) {
    auto sp = sizePolicy();
    sp.setHorizontalStretch(horizontal);
    sp.setVerticalStretch(vertical);
    setSizePolicy(sp);
}

GridPanel::GridPanel(rcstr title) : QGroupBox(title) {
    setLayout((grid_ = gridLayout()));
}

// ************************************************************************** //
//  final panel classes
// ************************************************************************** //

PanelReflection::PanelReflection() : GridPanel("Reflection") {
    auto g = grid();
    cbRefl = new QComboBox;
    cbRefl->addItems(gHub->reflectionsModel->names());
    g->addWidget(cbRefl);
    g->setRowStretch(g->rowCount(), 1);
}

PanelGammaSlices::PanelGammaSlices() : GridPanel("Gamma slices") {
    auto g = grid();

    g->addWidget(label("count"), 0, 0);
    g->addWidget((numSlices = spinCell(gui_cfg::em4, 0)), 0, 1);

    g->addWidget(label("degrees"), 1, 0);
    g->addWidget((stepGamma = spinDoubleCell(gui_cfg::em4_2, 0.0)), 1, 1);
    stepGamma->setReadOnly(true);

    g->setRowStretch(g->rowCount(), 1);

    rgeGma_ = gSession->experiment().rgeGma();

    connect(numSlices, slot(QSpinBox, valueChanged, int), [this]() { updateValues(); });
}

void PanelGammaSlices::updateValues() {
    uint nSlices = to_u(numSlices->value());
    if (nSlices > 0)
        stepGamma->setValue(rgeGma_.width() / nSlices);
    else
        stepGamma->clear();
}

PanelGammaRange::PanelGammaRange() : GridPanel("Gamma range") {
    auto g = grid();

    g->addWidget((cbLimitGamma = check("limit")), 0, 0, 1, 2);

    g->addWidget(label("min"), 1, 0);
    g->addWidget((minGamma = spinDoubleCell(gui_cfg::em4_2, -180., 180.)), 1, 1);

    g->addWidget(label("max"), 2, 0);
    g->addWidget((maxGamma = spinDoubleCell(gui_cfg::em4_2, -180., 180.)), 2, 1);

    g->setRowStretch(g->rowCount(), 1);

    rgeGma_ = gSession->experiment().rgeGma();

    minGamma->setValue(rgeGma_.min);
    maxGamma->setValue(rgeGma_.max);

    connect(cbLimitGamma, &QCheckBox::toggled, [this]() { updateValues(); });
}

// TODO when min/maxGamma updated -> reflect that in PanelGammaSlices

void PanelGammaRange::updateValues() {
    bool on = cbLimitGamma->isChecked();
    minGamma->setEnabled(on);
    maxGamma->setEnabled(on);
}

PanelPoints::PanelPoints() : GridPanel("Points") {
    auto g = grid();
    g->addWidget((rbCalc = radioButton("calculated")), 0, 0);
    g->addWidget((rbInterp = radioButton("interpolated")), 1, 0);

    g->setRowStretch(g->rowCount(), 1);
}

PanelInterpolation::PanelInterpolation() : GridPanel("Interpolation") {
    auto g = grid();

    g->addWidget(label("step α"), 0, 0, Qt::AlignRight);
    g->addWidget((stepAlpha = spinDoubleCell(gui_cfg::em4_2, 1., 30.)), 0, 1);
    g->addWidget(label("β"), 1, 0, Qt::AlignRight);
    g->addWidget((stepBeta = spinDoubleCell(gui_cfg::em4_2, 1., 30.)), 1, 1);
    g->addWidget(label("idw radius"), 2, 0, Qt::AlignRight);
    g->addWidget((idwRadius = spinDoubleCell(gui_cfg::em4_2, 0., 90.)), 2, 1);

    g->addWidget(label("avg. α max"), 0, 2, Qt::AlignRight);
    g->addWidget((avgAlphaMax = spinDoubleCell(gui_cfg::em4_2, 0., 90.)), 0, 3);
    g->addWidget(label("radius"), 1, 2, Qt::AlignRight);
    g->addWidget((avgRadius = spinDoubleCell(gui_cfg::em4_2, 0., 90.)), 1, 3);
    g->addWidget(label("inclusion %"), 2, 2, Qt::AlignRight);
    g->addWidget((avgThreshold = spinCell(gui_cfg::em4_2, 0, 100)), 2, 3);

    g->setRowStretch(g->rowCount(), 1);
}

PanelDiagram::PanelDiagram() : GridPanel("Diagram") {
    auto tags = ReflectionInfo::dataTags(false);
    for_i (Metadata::numAttributes(false) - Metadata::numAttributes(true))
        tags.removeLast(); // remove all tags that are not numbers

    auto g = grid();
    xAxis = (new QComboBox); xAxis->addItems(tags);
    yAxis = (new QComboBox); yAxis->addItems(tags);
    g->addWidget(label("x"), 1, 0);
    g->addWidget(xAxis, 1, 1);
    g->addWidget(label("y"), 0, 0);
    g->addWidget(yAxis, 0, 1);

    g->setRowStretch(g->rowCount(), 1);
}

PanelFitError::PanelFitError() : GridPanel("Fit error") {}

} // namespace output
} // namespace gui

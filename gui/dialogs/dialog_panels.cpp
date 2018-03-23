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

GridPanel::GridPanel(const QString& title) : QGroupBox(title)
{
    setLayout(&grid_);
}

void GridPanel::setHorizontalStretch(int stretch)
{
    QSizePolicy sp = sizePolicy();
    sp.setHorizontalStretch(stretch);
    setSizePolicy(sp);
}

void GridPanel::setVerticalStretch(int stretch)
{
    QSizePolicy sp = sizePolicy();
    sp.setVerticalStretch(stretch);
    setSizePolicy(sp);
}

void GridPanel::setStretch(int horizontal, int vertical)
{
    QSizePolicy sp = sizePolicy();
    sp.setHorizontalStretch(horizontal);
    sp.setVerticalStretch(vertical);
    setSizePolicy(sp);
}

// ************************************************************************** //
//  final panel classes
// ************************************************************************** //

PanelPeak::PanelPeak()
    : GridPanel("Peak#")
    , cbRefl("outRefl#", gSession->peaks().names())
{
    grid_.addWidget(&cbRefl);
    grid_.setRowStretch(grid_.rowCount(), 1);
}


PanelGammaSlices::PanelGammaSlices()
    : GridPanel("Gamma slices")
    , settings_("gamma_slices")
{
    grid_.addWidget(new QLabel("count"), 0, 0);
    numSlices.setValue(settings_.readInt("num_slices", 0));
    grid_.addWidget(&numSlices, 0, 1);

    grid_.addWidget(new QLabel("degrees"), 1, 0);
    grid_.addWidget(&stepGamma, 1, 1);
    stepGamma.setReadOnly(true);

    grid_.setRowStretch(grid_.rowCount(), 1);

    rgeGma_ = gSession->activeClusters().rgeGma();

    connect(&numSlices, &CSpinBox::valueReleased, [this]() { updateValues(); });
}

void PanelGammaSlices::updateValues()
{
    int nSlices = numSlices.value();
    if (nSlices > 0)
        stepGamma.setValue(rgeGma_.width() / nSlices);
    else
        stepGamma.clear();
}


PanelGammaRange::PanelGammaRange()
    : GridPanel("Gamma range")
    , settings_("gamma_range")
{
    grid_.addWidget(&cbLimitGamma, 0, 0, 1, 2);
    cbLimitGamma.setChecked(settings_.readBool("limit", false));

    grid_.addWidget(new QLabel("min"), 1, 0); grid_.addWidget(&minGamma, 1, 1);
    grid_.addWidget(new QLabel("max"), 2, 0); grid_.addWidget(&maxGamma, 2, 1);
    grid_.setRowStretch(grid_.rowCount(), 1);

    rgeGma_ = gSession->activeClusters().rgeGma();

    minGamma.setValue(rgeGma_.min);
    maxGamma.setValue(rgeGma_.max);

    connect(&cbLimitGamma, &QCheckBox::toggled, [this]() { updateValues(); });
}

// TODO when min/maxGamma updated -> reflect that in PanelGammaSlices

void PanelGammaRange::updateValues()
{
    bool on = cbLimitGamma.isChecked();
    minGamma.setEnabled(on);
    maxGamma.setEnabled(on);
}


PanelPoints::PanelPoints()
    : GridPanel("Points#")
    , settings_("polediagram_points")
{
    grid_.addWidget(&rbCalc, 0, 0);
    grid_.addWidget(&rbInterp, 1, 0);
    (settings_.readBool("interpolated", false) ? rbInterp : rbCalc).setChecked(true);

    grid_.setRowStretch(grid_.rowCount(), 1);
}


PanelInterpolation::PanelInterpolation()
    : GridPanel("Interpolation#")
{
    grid_.addWidget(new QLabel("step α"), 0, 0, Qt::AlignRight);
    grid_.addWidget(&stepAlpha, 0, 1);
    grid_.addWidget(new QLabel("β"), 1, 0, Qt::AlignRight);
    grid_.addWidget(&stepBeta, 1, 1);
    grid_.addWidget(new QLabel("idw radius"), 2, 0, Qt::AlignRight);
    grid_.addWidget(&idwRadius, 2, 1);
    grid_.addWidget(new QLabel("avg. α max"), 0, 2, Qt::AlignRight);
    grid_.addWidget(&avgAlphaMax, 0, 3);
    grid_.addWidget(new QLabel("radius"), 1, 2, Qt::AlignRight);
    grid_.addWidget(&avgRadius, 1, 3);
    grid_.addWidget(new QLabel("inclusion %"), 2, 2, Qt::AlignRight);
    grid_.addWidget(&avgThreshold, 2, 3);

    grid_.setRowStretch(grid_.rowCount(), 1);

    stepAlpha.setValue(settings_.readReal("step alpha", 5));
    stepBeta.setValue(settings_.readReal("step beta", 5));
    idwRadius.setValue(settings_.readReal("idw radius", 10));

    avgAlphaMax.setValue(settings_.readReal("avg alpha max", 15));
    avgRadius.setValue(settings_.readReal("avg radius", 5));
    avgThreshold.setValue(settings_.readInt("avg threshold", 100));
}

PanelDiagram::PanelDiagram()
    : GridPanel("Diagram#")
{
    QStringList tags = PeakInfo::dataTags(false);
    for_i (Metadata::numAttributes(false) - Metadata::numAttributes(true))
        tags.removeLast(); // remove all tags that are not numbers
    xAxis.addItems(tags);
    yAxis.addItems(tags);

    grid_.addWidget(new QLabel("x"), 1, 0);
    grid_.addWidget(&xAxis, 1, 1);
    grid_.addWidget(new QLabel("y"), 0, 0);
    grid_.addWidget(&yAxis, 0, 1);

    grid_.setRowStretch(grid_.rowCount(), 1);
}

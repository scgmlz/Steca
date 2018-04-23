//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_interpolation.cpp
//! @brief     Implements class ControlsInterpolation
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "controls_interpolation.h"
#include "core/session.h"

ControlsInterpolation::ControlsInterpolation()
{
    // inbound connection
    connect(gSession, &Session::sigInterpol, this, &ControlsInterpolation::fromCore);

    // outbound connections
    connect(&doInterpol_, &QCheckBox::toggled, [](bool on) {
            gSession->interpol().setEnabled(on); });
    connect(&stepAlpha_, &QcrDoubleSpinBox::valueReleased, [](double val) {
            gSession->interpol().setStepAlpha(val); });
    connect(&avgAlphaMax_, &QcrDoubleSpinBox::valueReleased, [](double val) {
            gSession->interpol().setAvgAlphaMax(val); });
    connect(&stepBeta_, &QcrDoubleSpinBox::valueReleased, [](double val) {
            gSession->interpol().setStepBeta(val); });
    connect(&avgRadius_, &QcrDoubleSpinBox::valueReleased, [](double val) {
            gSession->interpol().setAvgRadius(val); });
    connect(&idwRadius_, &QcrDoubleSpinBox::valueReleased, [](double val) {
            gSession->interpol().setIdwRadius(val); });
    connect(&threshold_, &QcrSpinBox::valueReleased, [](int val) {
            gSession->interpol().setThreshold(val); });

    // layout
    auto* grid = new QGridLayout;
    grid->addWidget(&doInterpol_,              0, 1);
    grid->addWidget(new QLabel("step α"),      1, 0, Qt::AlignRight);
    grid->addWidget(&stepAlpha_,               1, 1);
    grid->addWidget(new QLabel("avg. α max"),  2, 0, Qt::AlignRight);
    grid->addWidget(&avgAlphaMax_,             2, 1);
    grid->addWidget(new QLabel("β"),           3, 0, Qt::AlignRight);
    grid->addWidget(&stepBeta_,                3, 1);
    grid->addWidget(new QLabel("radius"),      4, 0, Qt::AlignRight);
    grid->addWidget(&avgRadius_,               4, 1);
    grid->addWidget(new QLabel("idw radius"),  5, 0, Qt::AlignRight);
    grid->addWidget(&idwRadius_,               5, 1);
    grid->addWidget(new QLabel("inclusion %"), 6, 0, Qt::AlignRight);
    grid->addWidget(&threshold_,               6, 1);

    grid->setColumnStretch(grid->columnCount(), 1000);
    grid->setRowStretch   (grid->   rowCount(), 1000);
    setLayout(grid);

    fromCore();
}


void ControlsInterpolation::fromCore()
{
    doInterpol_ .setChecked(gSession->interpol().enabled());
    stepAlpha_  .programaticallySetValue(gSession->interpol().stepAlpha());
    stepBeta_   .programaticallySetValue(gSession->interpol().stepBeta());
    idwRadius_  .programaticallySetValue(gSession->interpol().idwRadius());
    avgAlphaMax_.programaticallySetValue(gSession->interpol().avgAlphaMax());
    avgRadius_  .programaticallySetValue(gSession->interpol().avgRadius());
    threshold_  .programaticallySetValue(gSession->interpol().threshold());

    bool on = gSession->interpol().enabled();
    stepAlpha_  .setEnabled(on);
    stepBeta_   .setEnabled(on);
    idwRadius_  .setEnabled(on);
    avgAlphaMax_.setEnabled(on);
    avgRadius_  .setEnabled(on);
    threshold_  .setEnabled(on);
}

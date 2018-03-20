// ************************************************************************** //
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
// ************************************************************************** //

#include "controls_interpolation.h"
#include "core/session.h"
#include "gui/base/model_view.h"
#include "gui/mainwin.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"

ControlsInterpolation::ControlsInterpolation()
{
    // inbound connection
    connect(gSession, &Session::sigInterpol, this, &ControlsInterpolation::fromCore);

    // outbound connections
    connect(&stepAlpha_, _SLOT_(QDoubleSpinBox, valueChanged, double), [](double val) {
            gSession->interpol().setStepAlpha(val); });
    connect(&avgAlphaMax_, _SLOT_(QDoubleSpinBox, valueChanged, double), [](double val) {
            gSession->interpol().setAvgAlphaMax(val); });
    connect(&stepBeta_, _SLOT_(QDoubleSpinBox, valueChanged, double), [](double val) {
            gSession->interpol().setStepBeta(val); });
    connect(&avgRadius_, _SLOT_(QDoubleSpinBox, valueChanged, double), [](double val) {
            gSession->interpol().setAvgRadius(val); });
    connect(&idwRadius_, _SLOT_(QDoubleSpinBox, valueChanged, double), [](double val) {
            gSession->interpol().setIdwRadius(val); });
    connect(&threshold_, _SLOT_(QSpinBox, valueChanged, int), [](int val) {
            gSession->interpol().setThreshold(val); });

    // layout
    auto* grid = new QGridLayout;
    grid->addWidget(new QLabel("step α"), 0, 0, Qt::AlignRight);
    grid->addWidget(&stepAlpha_, 0, 1);
    grid->addWidget(new QLabel("avg. α max"), 1, 0, Qt::AlignRight);
    grid->addWidget(&avgAlphaMax_, 1, 1);
    grid->addWidget(new QLabel("β"), 2, 0, Qt::AlignRight);
    grid->addWidget(&stepBeta_, 2, 1);
    grid->addWidget(new QLabel("radius"), 3, 0, Qt::AlignRight);
    grid->addWidget(&avgRadius_, 3, 1);
    grid->addWidget(new QLabel("idw radius"), 4, 0, Qt::AlignRight);
    grid->addWidget(&idwRadius_, 4, 1);
    grid->addWidget(new QLabel("inclusion %"), 5, 0, Qt::AlignRight);
    grid->addWidget(&threshold_, 5, 1);

    grid->setColumnStretch(grid->columnCount(), 1000);
    grid->setRowStretch   (grid->   rowCount(), 1000);
    setLayout(grid);

    fromCore();
}

void ControlsInterpolation::fromCore()
{
    stepAlpha_  .setValue(gSession->interpol().stepAlpha());
    stepBeta_   .setValue(gSession->interpol().stepBeta());
    idwRadius_  .setValue(gSession->interpol().idwRadius());
    avgAlphaMax_.setValue(gSession->interpol().avgAlphaMax());
    avgRadius_  .setValue(gSession->interpol().avgRadius());
    threshold_  .setValue(gSession->interpol().threshold());
}

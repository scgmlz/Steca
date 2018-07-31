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
    : doInterpol_ {"doInterpol", "enabled", &gSession->interpolParams.enabled}
    , stepAlpha_  {"stepAlpha",  &gSession->interpolParams.stepAlpha,  6, 2, 1., 30.}
    , stepBeta_   {"stepBeta",   &gSession->interpolParams.stepBeta,   6, 2, 1., 30.}
    , idwRadius_  {"idwRadius",  &gSession->interpolParams.idwRadius,  6, 2, 0., 90.}
    , avgAlphaMax_{"avgAlphaMax",&gSession->interpolParams.avgAlphaMax,6, 2, 0., 90.}
    , avgRadius_  {"avgRadius",  &gSession->interpolParams.avgRadius,  6, 2, 0., 90.}
    , threshold_  {"threshold",  &gSession->interpolParams.threshold,  6, true, 0, 100}
{
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
    bool on = gSession->interpolParams.enabled.val();
    stepAlpha_  .setEnabled(on);
    stepBeta_   .setEnabled(on);
    idwRadius_  .setEnabled(on);
    avgAlphaMax_.setEnabled(on);
    avgRadius_  .setEnabled(on);
    threshold_  .setEnabled(on);
}

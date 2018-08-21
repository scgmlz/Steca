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

#include "gui/panels/controls_interpolation.h"
#include "core/session.h"
#include "qcr/widgets/controls.h"

ControlsInterpolation::ControlsInterpolation()
{
    auto& P = gSession->params;
    auto* doInterpol =
        new QcrCheckBox      {"doInterpol", "enabled", &P.interpolParams.enabled};
    auto* stepAlpha  =
        new QcrDoubleSpinBox {"stepAlpha",  &P.interpolParams.stepAlpha,  6, 2, 1., 30.};
    auto* stepBeta   =
        new QcrDoubleSpinBox {"stepBeta",   &P.interpolParams.stepBeta,   6, 2, 1., 30.};
    auto* idwRadius  =
        new QcrDoubleSpinBox {"idwRadius",  &P.interpolParams.idwRadius,  6, 2, 0., 90.};
    auto* avgAlphaMax=
        new QcrDoubleSpinBox {"avgAlphaMax",&P.interpolParams.avgAlphaMax,6, 2, 0., 90.};
    auto* avgRadius  =
        new QcrDoubleSpinBox {"avgRadius",  &P.interpolParams.avgRadius,  6, 2, 0., 90.};
    auto* threshold  =
        new QcrSpinBox       {"threshold",  &P.interpolParams.threshold,  6, true, 0, 100};

    auto* grid = new QGridLayout;
    grid->addWidget(doInterpol,              0, 1);
    grid->addWidget(new QLabel("step α"),      1, 0, Qt::AlignRight);
    grid->addWidget(stepAlpha,               1, 1);
    grid->addWidget(new QLabel("avg. α max"),  2, 0, Qt::AlignRight);
    grid->addWidget(avgAlphaMax,             2, 1);
    grid->addWidget(new QLabel("β"),           3, 0, Qt::AlignRight);
    grid->addWidget(stepBeta,                3, 1);
    grid->addWidget(new QLabel("radius"),      4, 0, Qt::AlignRight);
    grid->addWidget(avgRadius,               4, 1);
    grid->addWidget(new QLabel("idw radius"),  5, 0, Qt::AlignRight);
    grid->addWidget(idwRadius,               5, 1);
    grid->addWidget(new QLabel("inclusion %"), 6, 0, Qt::AlignRight);
    grid->addWidget(threshold,               6, 1);

    grid->setColumnStretch(grid->columnCount(), 1000);
    grid->setRowStretch   (grid->   rowCount(), 1000);
    setLayout(grid);

    setRemake([=](){
            bool on = gSession->params.interpolParams.enabled.val();
            stepAlpha  ->setEnabled(on);
            stepBeta   ->setEnabled(on);
            idwRadius  ->setEnabled(on);
            avgAlphaMax->setEnabled(on);
            avgRadius  ->setEnabled(on);
            threshold  ->setEnabled(on);
        });
}

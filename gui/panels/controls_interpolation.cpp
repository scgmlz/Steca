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


// ************************************************************************** //
//  class ControlsInterpolation
// ************************************************************************** //

ControlsInterpolation::ControlsInterpolation()
{
    grid_.addWidget(new QLabel("step α"), 0, 0, Qt::AlignRight);
    grid_.addWidget(&stepAlpha_, 0, 1);
    grid_.addWidget(new QLabel("avg. α max"), 1, 0, Qt::AlignRight);
    grid_.addWidget(&avgAlphaMax_, 1, 1);
    grid_.addWidget(new QLabel("β"), 2, 0, Qt::AlignRight);
    grid_.addWidget(&stepBeta_, 2, 1);
    grid_.addWidget(new QLabel("radius"), 3, 0, Qt::AlignRight);
    grid_.addWidget(&avgRadius_, 3, 1);
    grid_.addWidget(new QLabel("idw radius"), 4, 0, Qt::AlignRight);
    grid_.addWidget(&idwRadius_, 4, 1);
    grid_.addWidget(new QLabel("inclusion %"), 5, 0, Qt::AlignRight);
    grid_.addWidget(&avgThreshold_, 5, 1);

    grid_.setColumnStretch(grid_.columnCount(), 1000);
    grid_.setRowStretch   (grid_.   rowCount(), 1000);

    XSettings settings_("Interpolation");

    stepAlpha_.setValue(settings_.readReal("step alpha", 5));
    stepBeta_.setValue(settings_.readReal("step beta", 5));
    idwRadius_.setValue(settings_.readReal("idw radius", 10));

    avgAlphaMax_.setValue(settings_.readReal("avg alpha max", 15));
    avgRadius_.setValue(settings_.readReal("avg radius", 5));
    avgThreshold_.setValue(settings_.readInt("avg threshold", 100));

    setLayout(&grid_);

    /*
    connect(&spinDegree_, _SLOT_(QSpinBox, valueChanged, int), [](int degree_) {
            gSession->interpolation().setPolynomDegree(degree_); });

    connect(gSession, &Session::sigInterpolation, [this]() {
            spinDegree_.setValue(gSession->interpolation().polynomDegree()); });
    */
}

/* toSession
    settings_.saveReal("step alpha", stepAlpha.value());
    settings_.saveReal("step beta", stepBeta.value());
    settings_.saveReal("idw radius", idwRadius.value());
    settings_.saveReal("avg alpha max", avgAlphaMax.value());
    settings_.saveReal("avg radius", avgRadius.value());
    settings_.saveInt("avg threshold", avgThreshold.value());
*/

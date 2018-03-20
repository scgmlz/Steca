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
    hb_.addWidget(new QLabel("blabla"));
    hb_.addStretch(1);
    box_.addLayout(&hb_);
    setLayout(&box_);

    /*
    connect(&spinDegree_, _SLOT_(QSpinBox, valueChanged, int), [](int degree_) {
            gSession->interpolation().setPolynomDegree(degree_); });

    connect(gSession, &Session::sigInterpolation, [this]() {
            spinDegree_.setValue(gSession->interpolation().polynomDegree()); });
    */
}

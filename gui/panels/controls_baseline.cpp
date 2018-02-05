// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_baseline.cpp
//! @brief     Implements class ControlsBaseline
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "controls_baseline.h"
#include "core/session.h"
#include "gui/base/model_view.h"
#include "gui/mainwin.h"
#include <QStackedWidget>

ControlsBaseline::ControlsBaseline()
    : spinDegree_("spinDegree_", 4, false, 0, MainWin::MAX_POLYNOM_DEGREE)
{
    auto* box = newQ::VBoxLayout();
    setLayout(box);

    QBoxLayout* hb = newQ::HBoxLayout();
    box->addLayout(hb);
    hb->addWidget(newQ::IconButton(gHub->toggle_selRegions));
    hb->addWidget(newQ::IconButton(gHub->toggle_showBackground));
    hb->addWidget(newQ::IconButton(gHub->trigger_clearBackground));
    hb->addWidget(newQ::Label("Pol. degree:"));
    hb->addWidget(&spinDegree_);
    connect(&spinDegree_, _SLOT_(QSpinBox, valueChanged, int), [this](int degree) {
            gSession->baseline().setPolynomDegree(degree); });
    connect(gSession, &Session::sigBaseline, [this]() {
            spinDegree_.setValue(gSession->baseline().polynomDegree()); });
    hb->addStretch();

    box->addStretch(1);
}

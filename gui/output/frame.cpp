// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/frame.cpp
//! @brief     Implements class Frame
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "output_dialogs.h"
#include "actions.h"
#include "dialog_panels.h"
#include "calc/calc_polefigure.h"
#include "settings.h"
#include "session.h"
#include "def/idiomatic_for.h"
#include "signalling.h"
#include "frame.h"
#include "widget_makers.h"
#include <QAction>
#include <QProgressBar>

namespace gui {
namespace output {

Frame::Frame(rcstr title, Params* params, QWidget* parent)
    : QFrame(parent, Qt::Dialog) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle(title);
    setLayout((box_ = vbox()));

    debug::ensure(params);

    box_->addWidget((params_ = params));
    box_->addWidget((tabs_ = new panel::TabsPanel()));
    box_->setStretch(box_->count() - 1, 1);

    auto hb = hbox();
    box_->addLayout(hb);

    actClose_ = newTrigger("Close");
    actCalculate_ = newTrigger("Calculate");
    actInterpolate_ = newTrigger("Interpolate");

    hb->addWidget((btnClose_ = textButton(actClose_)));
    hb->addStretch(1);
    hb->addWidget((pb_ = new QProgressBar));
    hb->setStretchFactor(pb_, 333);
    hb->addStretch(1);
    hb->addWidget((btnCalculate_ = textButton(actCalculate_)));
    hb->addWidget((btnInterpolate_ = textButton(actInterpolate_)));

    pb_->hide();

    connect(actClose_, &QAction::triggered, [this]() { close(); });
    connect(actCalculate_, &QAction::triggered, [this]() { calculate(); });
    connect(actInterpolate_, &QAction::triggered, [this]() { interpolate(); });

    auto updateDisplay = [this]() { displayReflection(getReflIndex(), getInterpolated()); };

    if (params_->panelReflection) {
        connect(
            params_->panelReflection->cbRefl, slot(QComboBox, currentIndexChanged, int),
            [updateDisplay]() { updateDisplay(); });
    }

    if (params_->panelPoints) {
        debug::ensure(params_->panelReflection);
        connect(params_->panelPoints->rbInterp, &QRadioButton::toggled, [updateDisplay]() {
            updateDisplay();
        });
    }

    // tabs

    auto tabTable = new TabTable(
        *params_, calc::ReflectionInfo::dataTags(false), calc::ReflectionInfo::dataTags(true),
        calc::ReflectionInfo::dataCmps());
    tabs_->addTab("Points", Qt::Vertical).box().addWidget(tabTable);

    table_ = tabTable->table;

    uint reflCount = gSession->reflections().count();
    calcPoints_.resize(reflCount);
    interpPoints_.resize(reflCount);
}

void Frame::calculate() {
    TakesLongTime __;

    calcPoints_.clear();
    interpPoints_.clear();

    auto& reflections = gSession->reflections();
    if (!reflections.isEmpty()) {
        uint reflCount = reflections.count();

        auto ps = params_->panelGammaSlices;
        debug::ensure(ps);

        uint gammaSlices = to_u(ps->numSlices->value());

        auto pr = params_->panelGammaRange;
        debug::ensure(pr);

        typ::Range rgeGamma;
        if (pr->cbLimitGamma->isChecked())
            rgeGamma.safeSet(pr->minGamma->value(), pr->maxGamma->value());

        Progress progress(reflCount, pb_);

        for_i (reflCount)
            calcPoints_.append(
                gSession->makeReflectionInfos(
                    gSession->collectedDatasets(), *reflections.at(i),
                    gammaSlices, rgeGamma, &progress));
    }

    interpolate();
}

void Frame::interpolate() {
    TakesLongTime __;

    interpPoints_.clear();

    auto pi = params_->panelInterpolation;
    if (pi) {
        typ::deg alphaStep = pi->stepAlpha->value();
        typ::deg betaStep = pi->stepBeta->value();
        qreal idwRadius = pi->idwRadius->value();

        qreal avgRadius = pi->avgRadius->value();
        qreal avgAlphaMax = pi->avgAlphaMax->value();
        qreal avgTreshold = pi->avgThreshold->value() / 100.0;

        Progress progress(calcPoints_.count(), pb_);

        for_i (calcPoints_.count())
            interpPoints_.append(calc::interpolate(
                calcPoints_.at(i), alphaStep, betaStep, idwRadius, avgAlphaMax, avgRadius,
                avgTreshold, &progress));
    } else {
        for_i (calcPoints_.count())
            interpPoints_.append(calc::ReflectionInfos());
    }

    displayReflection(getReflIndex(), getInterpolated());
}

void Frame::displayReflection(uint reflIndex, bool interpolated) {
    table_->clear();

    debug::ensure(calcPoints_.count() == interpPoints_.count());
    if (calcPoints_.count() <= reflIndex)
        return;

    for (auto& r : (interpolated ? interpPoints_ : calcPoints_).at(reflIndex))
        table_->addRow(r.data(), false);

    table_->sortData();
}

uint Frame::getReflIndex() const {
    debug::ensure(params_->panelReflection);
    int reflIndex = params_->panelReflection->cbRefl->currentIndex();
    RUNTIME_CHECK(reflIndex >= 0, "invalid reflection index");
    return to_u(reflIndex);
}

bool Frame::getInterpolated() const {
    auto pi = params_->panelPoints;
    return pi ? pi->rbInterp->isChecked() : false;
}

} // namespace output
} // namespace gui

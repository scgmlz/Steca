// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_diffractogram.cpp
//! @brief     Implements class SubframeDiffractogram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "subframe_diffractogram.h"
#include "core/session.h"
#include "gui/panels/panel_diffractogram.h"
#include "gui/base/controls.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"


// ************************************************************************** //
//  local class Diffractogram
// ************************************************************************** //

//! A diffractogram display, with associated controls, for use in SubframeDiffractogram.

class Diffractogram final : public QWidget {
public:
    Diffractogram();
private:
    void onNormChanged();
    void onHighlight();

    class DiffractogramPlot* plot_;
    QVBoxLayout box_;
    QHBoxLayout hb_;
    CComboBox comboNormType_ {"normTyp", {"none", "monitor", "Δ monitor", "Δ time", "background"}};
    CRadioButton intenSum_ {"intenSum", "sum"};
    CRadioButton intenAvg_ {"intenAvg", "avg ×"};
    CDoubleSpinBox intenScale_ {"intenScale", 6, 0.001};
    CToggle actZoom_ {"actZoom", "zoom", false, ":/icon/zoom"};
    XIconButton enableZoom_ {&actZoom_};
    XIconButton combine_ {&gGui->toggles->combinedDgram};
    XIconButton fixInten_ {&gGui->toggles->fixedIntenDgram};
    XIconButton showBg_ {&gGui->toggles->showBackground};
    XIconButton export_ {&gGui->triggers->outputDiffractograms};
};


Diffractogram::Diffractogram()
{
    setLayout(&box_);
    box_.addWidget((plot_ = new DiffractogramPlot(*this)));
    box_.addLayout(&hb_);

    hb_.addWidget(new QLabel("normalize to:"));
    hb_.addWidget(&comboNormType_);

    connect(&comboNormType_, _SLOT_(QComboBox, currentIndexChanged, int), [](int index) {
            gSession->setNorm(eNorm(index)); });

    hb_.addWidget(new QLabel(" intensity from:"));
    hb_.addWidget(&intenSum_);
    hb_.addWidget(&intenAvg_);
    hb_.addWidget(&intenScale_);
    intenScale_.setDecimals(3);
    hb_.addStretch();

    connect(&intenAvg_, &QRadioButton::toggled, [this](bool on) {
        intenScale_.setEnabled(on);
        intenScale_.setValue(gSession->intenScale());
        gSession->setIntenScaleAvg(on, intenScale_.value());
    });

    connect(&intenScale_, &CDoubleSpinBox::valueReleased, [](double val) {
        if (val > 0)
            gSession->setIntenScaleAvg(gSession->intenScaledAvg(), val);
    });

    hb_.addWidget(&enableZoom_);
    hb_.addStretch();

    hb_.addWidget(&combine_);
    hb_.addWidget(&fixInten_);
    hb_.addWidget(&showBg_);
    hb_.addStretch();

    hb_.addWidget(&export_);

    connect(&actZoom_, &QAction::toggled, this, [this](bool on) {
        plot_->setInteraction(QCP::iRangeDrag, on);
        plot_->setInteraction(QCP::iRangeZoom, on);
        plot_->enterZoom(on);
    });

    connect(gSession, &Session::sigDataHighlight, this, &Diffractogram::onHighlight);
    connect(gSession, &Session::sigNorm, this, &Diffractogram::onNormChanged);

    gGui->toggles->showBackground.setChecked(true);
    intenAvg_.setChecked(true);
}

void Diffractogram::onNormChanged()
{
    intenScale_.setValue(gSession->intenScale()); // TODO own signal
    if (gSession->intenScaledAvg())
        intenAvg_.setChecked(true);
    else
        intenSum_.setChecked(true);
    plot_->renderAll();
}

void Diffractogram::onHighlight()
{
    actZoom_.setChecked(false);
    plot_->renderAll();
}

// ************************************************************************** //
//  class SubframeDiffractogram
// ************************************************************************** //

SubframeDiffractogram::SubframeDiffractogram()
{
    setTabPosition(QTabWidget::North);
    auto* tab = new Diffractogram();
    addTab(tab, "Diffractogram");
}

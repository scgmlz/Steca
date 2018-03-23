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

class Diffractogram : public QWidget {
public:
    Diffractogram();
private:
    void onNormChanged();
    void onHighlight();

    class DiffractogramPlot* plot_;
    CComboBox comboNormType_ {"normTyp", {"none", "monitor", "Δ monitor", "Δ time", "background"}};
    CRadioButton intenSum_ {"intenSum", "sum"};
    CRadioButton intenAvg_ {"intenAvg", "avg ×"};
    CDoubleSpinBox intenScale_ {"intenScale", 6, 0.001};
    CToggle actZoom_ {"actZoom", "zoom", false, ":/icon/zoom"};
};


Diffractogram::Diffractogram()
{
    // initializations
    plot_ = new DiffractogramPlot(*this);
    gGui->toggles->showBackground.setChecked(true);
    intenAvg_.setChecked(true);
    intenScale_.setDecimals(3);

    // inbound connections
    connect(gSession, &Session::sigDataHighlight, this, &Diffractogram::onHighlight);
    connect(gSession, &Session::sigNorm, this, &Diffractogram::onNormChanged);

    // internal connections
    connect(&actZoom_, &QAction::toggled, this, [this](bool on) {
        plot_->setInteraction(QCP::iRangeDrag, on);
        plot_->setInteraction(QCP::iRangeZoom, on);
        plot_->enterZoom(on);
    });

    // outbound connections
    connect(&comboNormType_, qOverload<int>(&QComboBox::currentIndexChanged), [](int index) {
            gSession->setNormMode(eNorm(index)); });
    connect(&intenAvg_, &QRadioButton::toggled, [this](bool on) {
        intenScale_.setEnabled(on);
        intenScale_.setValue(gSession->intenScale());
        gSession->setIntenScaleAvg(on, intenScale_.value());
    });
    connect(&intenScale_, &CDoubleSpinBox::valueReleased, [](double val) {
        if (val > 0)
            gSession->setIntenScaleAvg(gSession->intenScaledAvg(), val);
    });

    // layout
    auto* hb = new QHBoxLayout;
    hb->addWidget(new QLabel("normalize to:"));
    hb->addWidget(&comboNormType_);
    hb->addWidget(new QLabel(" intensity from:"));
    hb->addWidget(&intenSum_);
    hb->addWidget(&intenAvg_);
    hb->addWidget(&intenScale_);
    hb->addStretch(); // ---
    hb->addWidget(new XIconButton {&actZoom_});
    hb->addStretch(); // ---
    hb->addWidget(new XIconButton {&gGui->toggles->combinedDgram});
    hb->addWidget(new XIconButton {&gGui->toggles->fixedIntenDgram});
    hb->addWidget(new XIconButton {&gGui->toggles->showBackground});
    hb->addStretch(); // ---
    hb->addWidget(new XIconButton {&gGui->triggers->exportDfgram});

    auto* box = new QVBoxLayout;
    box->addWidget(plot_);
    box->addLayout(hb);
    setLayout(box);
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
    addTab(new Diffractogram, "Diffractogram");
}

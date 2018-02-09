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

#include "gui/panels/subframe_diffractogram.h"
#include "gui/panels/panel_diffractogram.h"
#include "gui/base/layout.h"
#include "gui/base/controls.h"
#include "core/session.h"


// ************************************************************************** //
//  local class Diffractogram
// ************************************************************************** //

//! A diffractogram display, with associated controls, for use in SubframeDiffractogram.

class Diffractogram final : public QWidget {
public:
    Diffractogram();

    QBoxLayout* box() const { return box_; }

private:
    void onNormChanged();
    void onFittingTab(eFittingTab tab);

    void onHighlight();

    class DiffractogramPlot* plot_;
    QBoxLayout* box_;
    CComboBox comboNormType_{"normTyp", {"none", "monitor", "Δ monitor", "Δ time", "background"}};
    CRadioButton intenSum_{"intenSum", "sum"};
    CRadioButton intenAvg_{"intenAvg", "avg ×"};
    CDoubleSpinBox intenScale_{"intenScale", 6, 0.001};
    QAction* actZoom_{ newT::Toggle("actZoom", "zoom", false) };
    XTextButton enableZoom_{actZoom_};
    CCheckBox combine_{"dgram:combine", gGui->toggle_combinedDgram};
    CCheckBox fixInten_{"dgram:fixInten", gGui->toggle_fixedIntenDgram};
};


Diffractogram::Diffractogram() {

    setLayout((box_ = newQ::VBoxLayout()));
    box_->addWidget((plot_ = new DiffractogramPlot(*this)));
    auto hb = newQ::HBoxLayout();
    box_->addLayout(hb);

    hb->addWidget(new QLabel("normalize to:"));
    hb->addWidget(&comboNormType_);

    connect(&comboNormType_, _SLOT_(QComboBox, currentIndexChanged, int), [this](int index) {
            gSession->setNorm(eNorm(index)); });

    hb->addWidget(new QLabel(" intensity from:"));
    hb->addWidget(&intenSum_);
    hb->addWidget(&intenAvg_);
    hb->addWidget(&intenScale_);
    intenScale_.setDecimals(3);
    hb->addStretch();

    connect(&intenAvg_, &QRadioButton::toggled, [this](bool on) {
        intenScale_.setEnabled(on);
        intenScale_.setValue(gSession->intenScale());
        gSession->setIntenScaleAvg(on, intenScale_.value());
    });

    connect(&intenScale_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this](double val) {
        if (val > 0)
            gSession->setIntenScaleAvg(gSession->intenScaledAvg(), val);
    });


    hb->addWidget(&enableZoom_);
    hb->addStretch();

    hb->addWidget(&combine_);
    hb->addWidget(&fixInten_);

    connect(actZoom_, &QAction::toggled, this, [this](bool on) {
        plot_->setInteraction(QCP::iRangeDrag, on);
        plot_->setInteraction(QCP::iRangeZoom, on);
        plot_->enterZoom(on);
    });

    connect(gSession, &Session::sigDataHighlight, this, &Diffractogram::onHighlight);
    connect(gSession, &Session::sigNorm, this, &Diffractogram::onNormChanged);
    connect(gGui, &MainWin::sigFittingTab, [this](eFittingTab tab) { onFittingTab(tab); });

    connect(gGui->toggle_selRegions, &QAction::toggled, [this](bool on) {
        using eTool = DiffractogramPlot::eTool;
        if      (on && gGui->fittingTab()==eFittingTab::BACKGROUND)
            plot_->setTool(eTool::BACKGROUND);
        else if (on && gGui->fittingTab()==eFittingTab::REFLECTIONS)
            plot_->setTool(eTool::PEAK_REGION);
        else
            plot_->setTool(eTool::NONE);
        });

    gGui->toggle_selRegions->setChecked(true);
    gGui->toggle_showBackground->setChecked(true);
    intenAvg_.setChecked(true);
}

void Diffractogram::onNormChanged() {
    intenScale_.setValue(gSession->intenScale()); // TODO own signal
    if (gSession->intenScaledAvg())
        intenAvg_.setChecked(true);
    else
        intenSum_.setChecked(true);
    plot_->renderAll();
}

void Diffractogram::onFittingTab(eFittingTab tab) {
    bool on = gGui->toggle_selRegions->isChecked();
    switch (tab) {
    case eFittingTab::BACKGROUND:
        gGui->toggle_selRegions->setIcon(QIcon(":/icon/selRegion"));
        plot_->setTool(
            on ? DiffractogramPlot::eTool::BACKGROUND : DiffractogramPlot::eTool::NONE);
        break;
    case eFittingTab::REFLECTIONS:
        gGui->toggle_selRegions->setIcon(QIcon(":/icon/reflRegion"));
        plot_->setTool(
            on ? DiffractogramPlot::eTool::PEAK_REGION : DiffractogramPlot::eTool::NONE);
        break;
    default: plot_->setTool(DiffractogramPlot::eTool::NONE);
    }
}

void Diffractogram::onHighlight() {
    actZoom_->setChecked(false);
    plot_->renderAll();
}

// ************************************************************************** //
//  class SubframeDiffractogram
// ************************************************************************** //

SubframeDiffractogram::SubframeDiffractogram() {
    setTabPosition(QTabWidget::North);
    auto* tab = new QWidget();
    addTab(tab, "Diffractogram");
    tab->setLayout(newQ::VBoxLayout());
    tab->layout()->addWidget(new Diffractogram());
}

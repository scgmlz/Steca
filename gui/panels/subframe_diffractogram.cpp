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
#include "gui/base/new_q.h"
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
    QComboBox* comboNormType_;
    QRadioButton *intenSum_, *intenAvg_;
    QDoubleSpinBox* intenScale_;
    QToolButton* enableZoom_;
    QAction* actZoom_;
};


Diffractogram::Diffractogram() {

    setLayout((box_ = newQ::VBoxLayout()));
    box_->addWidget((plot_ = new DiffractogramPlot(*this)));
    auto hb = newQ::HBoxLayout();
    box_->addLayout(hb);

    hb->addWidget(newQ::Label("", "normalize to:"));
    comboNormType_ = new QComboBox;
    comboNormType_->addItems({"none", "monitor", "Δ monitor", "Δ time", "background"});
    hb->addWidget(comboNormType_);

    connect(comboNormType_, _SLOT_(QComboBox, currentIndexChanged, int),
            [this](int index) { // TODO init value from hub?
                gSession->setNorm(eNorm(index));
            });

    hb->addWidget(newQ::Label("", " intensity from:"));
    hb->addWidget((intenSum_ = newQ::RadioButton("intenSum_", "sum")));
    hb->addWidget((intenAvg_ = newQ::RadioButton("intenAvg_", "avg ×")));
    hb->addWidget((intenScale_ = newQ::DoubleSpinBox("intenScale_", 6, 0.001)));
    intenScale_->setDecimals(3);

    connect(intenAvg_, &QRadioButton::toggled, [this](bool on) {
        intenScale_->setEnabled(on);
        intenScale_->setValue(gSession->intenScale());
        gSession->setIntenScaleAvg(on, intenScale_->value());
    });

    connect(intenScale_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this](double val) {
        if (val > 0)
            gSession->setIntenScaleAvg(gSession->intenScaledAvg(), val);
    });

    hb->addStretch();

    actZoom_ = newQ::Toggle("actZoom_", "zoom", false);
    enableZoom_ = newQ::TextButton(actZoom_);
    hb->addWidget(enableZoom_);

    hb->addStretch();

    hb->addWidget(newQ::CheckBox("", gHub->toggle_combinedDgram));
    hb->addWidget(newQ::CheckBox("", gHub->toggle_fixedIntenDgram));

    connect(actZoom_, &QAction::toggled, this, [this](bool on) {
        plot_->setInteraction(QCP::iRangeDrag, on);
        plot_->setInteraction(QCP::iRangeZoom, on);
        plot_->enterZoom(on);
    });

    connect(gSession, &Session::sigHighlight, this, &Diffractogram::onHighlight);
    connect(gSession, &Session::sigNorm, this, &Diffractogram::onNormChanged);
    connect(gHub, &TheHub::sigFittingTab, [this](eFittingTab tab) { onFittingTab(tab); });

    connect(gHub->toggle_selRegions, &QAction::toggled, [this](bool on) {
        using eTool = DiffractogramPlot::eTool;
        auto tool = eTool::NONE;
        if (on)
            switch (gHub->fittingTab()) {
            case eFittingTab::BACKGROUND: tool = eTool::BACKGROUND; break;
            case eFittingTab::REFLECTIONS: tool = eTool::PEAK_REGION; break;
            default: break;
            }
        plot_->setTool(tool);
        });

    gHub->toggle_selRegions->setChecked(true);
    gHub->toggle_showBackground->setChecked(true);
    intenAvg_->setChecked(true);
}

void Diffractogram::onNormChanged() {
    intenScale_->setValue(gSession->intenScale()); // TODO own signal
    if (gSession->intenScaledAvg())
        intenAvg_->setChecked(true);
    else
        intenSum_->setChecked(true);
    plot_->renderAll();
}

void Diffractogram::onFittingTab(eFittingTab tab) {
    bool on = gHub->toggle_selRegions->isChecked();
    switch (tab) {
    case eFittingTab::BACKGROUND:
        gHub->toggle_selRegions->setIcon(QIcon(":/icon/selRegion"));
        plot_->setTool(
            on ? DiffractogramPlot::eTool::BACKGROUND : DiffractogramPlot::eTool::NONE);
        break;
    case eFittingTab::REFLECTIONS:
        gHub->toggle_selRegions->setIcon(QIcon(":/icon/reflRegion"));
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

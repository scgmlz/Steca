//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_dfgram.cpp
//! @brief     Implements class SubframeDfgram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "subframe_dfgram.h"
#include "core/session.h"
#include "gui/view/plot_dfgram.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"
#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

//  ***********************************************************************************************
//! @class Dfgram (local scope)

//! A diffractogram display, with associated controls, for use in SubframeDfgram.

class Dfgram : public QcrWidget {
public:
    Dfgram();
    Dfgram(const Dfgram&) = delete;
private:
    void onNormChanged();
    void onHighlight();

    class PlotDfgram* plot_;
    QcrComboBox comboNormType_;
    QcrRadioButton intenSum_;
    QcrRadioButton intenAvg_;
    QcrDoubleSpinBox intenScale_;
    QcrToggle actZoom_ {"actZoom", "zoom", false, ":/icon/zoom"};
};


Dfgram::Dfgram()
    : QcrWidget {"dfgram"}
    , comboNormType_ {"normTyp", {"none", "monitor", "Δ monitor", "time", "Δ time"}}
    , intenSum_ {"intenSum", "sum"}
    , intenAvg_ {"intenAvg", "avg ×", &gSession->intenScaledAvg}
    , intenScale_ {"intenScale", &gSession->intenScale, 4, 0.001}
{
    // initializations
    plot_ = new PlotDfgram(*this);
    intenAvg_.programaticallySetValue(true);

    // internal connections
    connect(&actZoom_, &QAction::toggled, this, [this](bool on) {
        plot_->setInteraction(QCP::iRangeDrag, on);
        plot_->setInteraction(QCP::iRangeZoom, on);
        plot_->enterZoom(on);
    });

    // outbound connections
    connect(&comboNormType_, _SLOT_(QComboBox,currentIndexChanged,int), [](int index) {
            gSession->setNormMode(eNorm(index)); });

    // layout
    auto* hb = new QHBoxLayout;
    hb->addWidget(new QLabel("normalize to:"));
    hb->addWidget(&comboNormType_);
    hb->addWidget(new QLabel(" intensity from:"));
    hb->addWidget(&intenSum_);
    hb->addWidget(&intenAvg_);
    hb->addWidget(&intenScale_);
    hb->addStretch(); // ---
    hb->addWidget(new QcrIconToggleButton {&actZoom_});
    hb->addStretch(); // ---
    hb->addWidget(new QcrIconToggleButton {&gGui->toggles->combinedDgram});
    hb->addWidget(new QcrIconToggleButton {&gGui->toggles->fixedIntenDgram});
    hb->addWidget(new QcrIconToggleButton {&gGui->toggles->showBackground});
    hb->addStretch(); // ---
    hb->addWidget(new QcrIconTriggerButton {&gGui->triggers->exportDfgram});

    auto* box = new QVBoxLayout;
    box->addWidget(plot_);
    box->addLayout(hb);
    setLayout(box);

    setRemake([this]() {plot_->renderAll();});
}

void Dfgram::onNormChanged()
{
    intenScale_.programaticallySetValue(gSession->intenScale.val()); // TODO own signal
    if (gSession->intenScaledAvg.val())
        intenAvg_.programaticallySetValue(true);
    else
        intenSum_.programaticallySetValue(true);
    plot_->renderAll();
}

void Dfgram::onHighlight() // TODO currently unused
{
    actZoom_.programaticallySetValue(false);
    plot_->renderAll();
}

//  ***********************************************************************************************
//! @class SubframeDfgram

SubframeDfgram::SubframeDfgram()
    : QcrTabWidget {"dfgramTabs"}
{
    setTabPosition(QTabWidget::North);
    addTab(new Dfgram, "Diffractogram");
}

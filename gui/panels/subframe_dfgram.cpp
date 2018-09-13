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

#include "gui/panels/subframe_dfgram.h"
#include "core/session.h"
#include "gui/view/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/view/plot_dfgram.h"
#include "gui/mainwin.h"

#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

//  ***********************************************************************************************
//! @class DfPanel (local scope)

//! A diffractogram display, with associated controls, for use in SubframeDfgram.

class DfPanel : public QcrWidget {
public:
    DfPanel();
    DfPanel(const DfPanel&) = delete;
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


DfPanel::DfPanel()
    : comboNormType_ {"normTyp", &gSession->params.howtoNormalize,
        []()->QStringList{return {"none", "monitor", "Δ monitor", "time", "Δ time"};}}
    , intenSum_ {"intenSum", "sum"}
    , intenAvg_ {"intenAvg", "avg ×", &gSession->params.intenScaledAvg}
    , intenScale_ {"intenScale", &gSession->params.intenScale, 5, 1, 0.001}
{
    plot_ = new PlotDfgram();
    intenAvg_.programaticallySetValue(true);

    actZoom_.setHook([this](bool on) {
        plot_->setInteraction(QCP::iRangeDrag, on);
        plot_->setInteraction(QCP::iRangeZoom, on);
        plot_->enterZoom(on); });

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
    hb->addWidget(new QcrIconToggleButton {&gGui->toggles->combinedDfgram});
    hb->addWidget(new QcrIconToggleButton {&gGui->toggles->fixedIntenDfgram});
    hb->addWidget(new QcrIconToggleButton {&gGui->toggles->showBackground});
    hb->addStretch(); // ---
    auto* btnExport = new QcrIconTriggerButton {&gGui->triggers->exportDfgram};
    hb->addWidget(btnExport);

    auto* box = new QVBoxLayout;
    box->addWidget(plot_);
    box->addLayout(hb);
    setLayout(box);

    setRemake([=](){
            btnExport->setEnabled(gSession->activeClusters.size());
            plot_->renderAll(); });
}

void DfPanel::onNormChanged()
{
    intenScale_.programaticallySetValue(gSession->params.intenScale.val()); // TODO own signal
    if (gSession->params.intenScaledAvg.val())
        intenAvg_.programaticallySetValue(true);
    else
        intenSum_.programaticallySetValue(true);
    plot_->renderAll();
}

void DfPanel::onHighlight() // TODO currently unused
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
    addTab(new DfPanel, "Diffractogram");
    setRemake([this](){setEnabled(gSession->activeClusters.size());});
}

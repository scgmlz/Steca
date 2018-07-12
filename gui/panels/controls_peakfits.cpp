//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_peakfits.cpp
//! @brief     Implements class ControlsPeakfits, and local classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "controls_peakfits.h"
#include "core/session.h"
#include "qcr/widgets/views.h"
#include "qcr/widgets/tables.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "gui/actions/triggers.h"
#include "gui/view/range_control.h"
#include "qcr/base/debug.h"
#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

namespace {
QString safeRealText(double val) { return qIsFinite(val) ? QString::number(val) : ""; }
} // local methods

//  ***********************************************************************************************
//! @class PeaksModel, used in PeaksView (local scope)

//! Model for PeaksView.

class PeaksModel : public TableModel {
public:
    PeaksModel() : TableModel("peaks") {}

    int columnCount() const final { return NUM_COLUMNS; }
    int rowCount() const final { return gSession->peaks().count(); }
    int highlighted() const final { return gSession->peaks().selectedIndex(); }
    void setHighlight(int row) final { gSession->peaks().select(row); }

    QVariant data(const QModelIndex&, int) const;

    enum { COL_ID = 1, COL_TYPE, COL_RANGE, NUM_COLUMNS };
};

QVariant PeaksModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row < 0 || rowCount() <= row)
        return {};
    const Peak& peak = gSession->peaks().at(row);
    switch (role) {
    case Qt::DisplayRole: {
        int col = index.column();
        if (col < 1)
            return {};
        switch (col) {
        case COL_ID:
            return QString::number(row + 1);
        case COL_TYPE:
            return peak.functionName();
        case COL_RANGE:
            if (!peak.range().isValid())
                return "undefined";
            else if (peak.range().isEmpty())
                return "empty";
            return peak.range().to_s();
        default:
            return {};
        }
    }
    case Qt::ForegroundRole: {
        if (peak.range().isEmpty())
            return QColor(Qt::red);
        return QColor(Qt::black);
    }
    case Qt::BackgroundRole: {
        if (row==highlighted())
            return QColor(Qt::cyan);
        return QColor(Qt::white);
    }
    default:
        return {};
    }
}


//  ***********************************************************************************************
//! @class PeaksView (local scope)

//! List view of user-defined Bragg peaks.

class PeaksView final : public TableView {
public:
    PeaksView();
private:
};

PeaksView::PeaksView()
    : TableView(new PeaksModel())
{
}

//  ***********************************************************************************************
//! @class ParamsView and its dependences (local scope)

//! Virtual base class for RawParamsView and FitParamsView.

class AnyParamsView : public QcrWidget {
public:
    AnyParamsView() = delete;
    AnyParamsView(const QString& name);
    virtual void updatePeakFun(const PeakFunction&);
protected:
    QGridLayout grid_;
    QcrLineDisplay readFitPeakX_ {"fittedX", 6, true};
    QcrLineDisplay readFitPeakY_ {"fittedY", 6, true};
    QcrLineDisplay readFitFWHM_  {"fittedW", 6, true};
};

AnyParamsView::AnyParamsView(const QString& name)
    : QcrWidget(name)
{
    setLayout(&grid_);
}

void AnyParamsView::updatePeakFun(const PeakFunction& peakFun)
{
    const qpair& fittedPeak = peakFun.fittedPeak();
    readFitPeakX_.setText(safeRealText(fittedPeak.x));
    readFitPeakY_.setText(safeRealText(fittedPeak.y));
    readFitFWHM_.setText(safeRealText(peakFun.fittedFWHM()));
}

//! Displays outcome of raw data analysis.

class RawParamsView : public AnyParamsView {
public:
    RawParamsView();
};

RawParamsView::RawParamsView()
    : AnyParamsView{"rawParamsView"}
{
    grid_.addWidget(new QLabel(""), 0, 0);

    grid_.addWidget(new QLabel("centre"), 1, 0);
    grid_.addWidget(&readFitPeakX_, 1, 2);
    grid_.addWidget(new QLabel("deg"), 1, 3);

    grid_.addWidget(new QLabel("fwhm"), 2, 0);
    grid_.addWidget(&readFitFWHM_, 2, 2);
    grid_.addWidget(new QLabel("deg"), 2, 3);

    grid_.addWidget(new QLabel("intens"), 3, 0);
    grid_.addWidget(&readFitPeakY_, 3, 2);

    grid_.setColumnStretch(4, 1);
}

//! Displays outcome of peak fit.

class FitParamsView : public AnyParamsView {
public:
    FitParamsView();
    virtual void updatePeakFun(const PeakFunction&);
private:
    QcrLineDisplay spinGuessPeakX_ {"guessedX", 6, true};
    QcrLineDisplay spinGuessPeakY_ {"guessedY", 6, true};
    QcrLineDisplay spinGuessFWHM_  {"guessedW", 6, true};
};

FitParamsView::FitParamsView()
    : AnyParamsView{"fitParamsView"}
{
    grid_.addWidget(new QLabel("guess"), 0, 1);
    grid_.addWidget(new QLabel("fitted"), 0, 2);

    grid_.addWidget(new QLabel("centre"), 1, 0);
    grid_.addWidget(&spinGuessPeakX_, 1, 1);
    grid_.addWidget(&readFitPeakX_, 1, 2);
    grid_.addWidget(new QLabel("deg"), 1, 3);

    grid_.addWidget(new QLabel("fwhm"), 2, 0);
    grid_.addWidget(&spinGuessFWHM_, 2, 1);
    grid_.addWidget(&readFitFWHM_, 2, 2);
    grid_.addWidget(new QLabel("deg"), 2, 3);

    grid_.addWidget(new QLabel("intens"), 3, 0);
    grid_.addWidget(&spinGuessPeakY_, 3, 1);
    grid_.addWidget(&readFitPeakY_, 3, 2);

    grid_.setColumnStretch(4, 1);
}

void FitParamsView::updatePeakFun(const PeakFunction& peakFun)
{
    AnyParamsView::updatePeakFun(peakFun);

    const qpair& guessedPeak = peakFun.guessedPeak();
    spinGuessPeakX_.setText(safeRealText(guessedPeak.x));
    spinGuessPeakY_.setText(safeRealText(guessedPeak.y));
    spinGuessFWHM_.setText(safeRealText(peakFun.guessedFWHM()));
}


//! Displays result of either raw data analysis or peak fit.

class ParamsView : public QcrStackedWidget {
public:
    ParamsView();
private:
    AnyParamsView* widgets_[2];
};

ParamsView::ParamsView()
    : QcrStackedWidget{"paramView"}
{
    addWidget(widgets_[0] = new RawParamsView());
    addWidget(widgets_[1] = new FitParamsView());
    widgets_[0]->show();
    setRemake( [=]() {
            Peak* peak = gSession->peaks().selectedPeak();
            setEnabled(peak);
            if (!peak)
                return;
            const PeakFunction& peakFun = peak->peakFunction();
            int i = peakFun.isRaw() ? 0 : 1;
            widgets_[i]->updatePeakFun(peakFun);
            setCurrentIndex(i);
        } );
}


//  ***********************************************************************************************
//! @class ControlsPeakfits

ControlsPeakfits::ControlsPeakfits()
    : QcrWidget{"peaks"}
    , comboReflType_ {"reflTyp", FunctionRegistry::instance()->keys()}
{
    // outbound connections
    connect(&gGui->triggers->peakRemove, &QAction::triggered, []() {
            gSession->peaks().removeSelected();
            gRoot->remakeAll("removePeak"); });
    connect(&gGui->triggers->peaksClear, &QAction::triggered, []() {
            gSession->peaks().clear();
            gRoot->remakeAll("clearPeaks"); });

    comboReflType_.cell()->setHook( [](int i) {
            const QString& peakFunctionName = FunctionRegistry::instance()->keys()[i];
            Peaks::defaultFunctionName = peakFunctionName;
            if (gSession->peaks().selectedPeak())
                gSession->peaks().selectedPeak()->setPeakFunction(peakFunctionName);
            gRoot->remakeAll("reflType");
        } );

    // layout
    topControls_.addStretch();
    topControls_.addWidget(new QcrIconTriggerButton(&gGui->triggers->peakAdd));
    topControls_.addWidget(new QcrIconTriggerButton(&gGui->triggers->peakRemove));
    topControls_.addWidget(new QcrIconTriggerButton(&gGui->triggers->peaksClear));

    auto* box = new QVBoxLayout;
    box->addLayout(&topControls_);
    box->addWidget(new PeaksView);
    box->addWidget(&comboReflType_);
    box->addWidget(new RangeControl("peak", []()->Range*{
                return gSession->peaks().selectedRange(); }));
    box->addWidget(new ParamsView);
    box->addStretch(1000);
    setLayout(box);
}

void ControlsPeakfits::hideEvent(QHideEvent*)
{
    gGui->state->editingPeakfits = false;
}

void ControlsPeakfits::showEvent(QShowEvent*)
{
    gGui->state->editingPeakfits = true;
}

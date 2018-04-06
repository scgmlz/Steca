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
#include "gui/base/displays.h"
#include "gui/base/model_view.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "gui/actions/triggers.h"
#include <QStackedWidget>

namespace {
double safeReal(double val) { return qIsFinite(val) ? val : 0.0; }
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
    void currentChanged(const QModelIndex& current, const QModelIndex&) override final {
        gotoCurrent(current); }
};

PeaksView::PeaksView()
    : TableView(new PeaksModel())
{
    connect(gSession, &Session::sigPeaks, this, &PeaksView::onData);
    connect(this, &TableView::clicked, model_, &TableModel::onClicked);
}

//  ***********************************************************************************************
//! @class RangeControl (local scope)

//! A horizontal row with labels and spin boxes to view and change one peak fit range.

class RangeControl : public QWidget {
public:
    RangeControl();
    void onData();
private:
    CDoubleSpinBox spinRangeMin_ {"peakRangeMin", 6, 0., 89.9};
    CDoubleSpinBox spinRangeMax_ {"peakRangeMax", 6, 0., 90.};
};

RangeControl::RangeControl()
{
    spinRangeMin_.setSingleStep(.1);
    spinRangeMax_.setSingleStep(.1);

    // inbound connections
    connect(gSession, &Session::sigPeaks, this, &RangeControl::onData);

    // outbound connections
    connect(&spinRangeMin_, &CDoubleSpinBox::valueReleased, [this](double val) {
            double antival = qMax(spinRangeMax_.value(), val);
            gSession->peaks().selectedPeak()->setRange(Range(val, antival)); });
    connect(&spinRangeMax_, &CDoubleSpinBox::valueReleased, [this](double val) {
            double antival = qMin(spinRangeMin_.value(), val);
            gSession->peaks().selectedPeak()->setRange(Range(antival, val)); });

    // layout
    auto hb = new QHBoxLayout();
    hb->addWidget(new QLabel("range"));
    hb->addWidget(&spinRangeMin_);
    hb->addWidget(new QLabel(".."));
    hb->addWidget(&spinRangeMax_);
    hb->addWidget(new QLabel("deg"));
    hb->addStretch();
    setLayout(hb);

    onData();
}

void RangeControl::onData()
{
    Peak* peak = gSession->peaks().selectedPeak();
    setEnabled(peak);
    if (!peak)
        return;
    Range range = peak->range();
    spinRangeMin_.setValue(safeReal(range.min));
    spinRangeMax_.setValue(safeReal(range.max));
}


//  ***********************************************************************************************
//! @class ParamsView and its dependences (local scope)

//! Virtual base class for RawParamsView and FitParamsView.

class AnyParamsView : public QWidget {
public:
    AnyParamsView();
    virtual void updatePeakFun(const PeakFunction&);
protected:
    QGridLayout grid_;
    XLineDisplay readFitPeakX_ {6, true};
    XLineDisplay readFitPeakY_ {6, true};
    XLineDisplay readFitFWHM_ {6, true};
};

AnyParamsView::AnyParamsView()
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
    XLineDisplay spinGuessPeakX_ {6, true};
    XLineDisplay spinGuessPeakY_ {6, true};
    XLineDisplay spinGuessFWHM_ {6, true};
};

FitParamsView::FitParamsView()
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

class ParamsView : public QStackedWidget {
public:
    ParamsView();
private:
    void onData();
    AnyParamsView* widgets_[2];
};

ParamsView::ParamsView()
{
    addWidget(widgets_[0] = new RawParamsView());
    addWidget(widgets_[1] = new FitParamsView());
    widgets_[0]->show();
    connect(gSession, &Session::sigPeaks, this, &ParamsView::onData);
    connect(gSession, &Session::sigRawFits, this, &ParamsView::onData);
    onData();
}

void ParamsView::onData()
{
    Peak* peak = gSession->peaks().selectedPeak();
    setEnabled(peak);
    if (!peak)
        return;
    const PeakFunction& peakFun = peak->peakFunction();
    int i = peakFun.isRaw() ? 0 : 1;
    widgets_[i]->updatePeakFun(peakFun);
    setCurrentIndex(i);
}


//  ***********************************************************************************************
//! @class ControlsPeakfits

ControlsPeakfits::ControlsPeakfits()
    : comboReflType_ {"reflTyp", FunctionRegistry::instance()->keys()}
{
    // inbound connection
    connect(gSession, &Session::sigPeaks, this, &ControlsPeakfits::onPeaks);

    // outbound connections
    connect(&gGui->triggers->addPeak, &QAction::triggered, [this]() {
            gSession->peaks().add(comboReflType_.currentText()); });
    connect(&gGui->triggers->removePeak, &QAction::triggered, []() {
            gSession->peaks().remove(); });
    connect(&comboReflType_, qOverload<const QString&>(&QComboBox::currentIndexChanged),
            [](const QString& peakFunctionName) {
                if (gSession->peaks().selectedPeak()) { // TODO rm this if
                    gSession->peaks().selectedPeak()->setPeakFunction(peakFunctionName);
                    EMIT(gSession->sigPeaks());
                } });

    // layout
    topControls_.addStretch();
    topControls_.addWidget(new XIconButton(&gGui->triggers->addPeak));
    topControls_.addWidget(new XIconButton(&gGui->triggers->removePeak));

    auto* box = new QVBoxLayout;
    box->addLayout(&topControls_);
    box->addWidget(new PeaksView);
    box->addWidget(&comboReflType_);
    box->addWidget(new RangeControl);
    box->addWidget(new ParamsView);
    box->addStretch(1000);

    setLayout(box);
    update();
}

void ControlsPeakfits::onPeaks()
{
    Peak* peak = gSession->peaks().selectedPeak();
    if (peak)
        comboReflType_.setCurrentText(peak->functionName());
};

void ControlsPeakfits::hideEvent(QHideEvent*)
{
    gGui->state->editingPeakfits = false;
}

void ControlsPeakfits::showEvent(QShowEvent*)
{
    gGui->state->editingPeakfits = true;
}

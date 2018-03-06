// ************************************************************************** //
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
// ************************************************************************** //

#include "controls_peakfits.h"
#include "core/session.h"
#include "gui/base/displays.h"
#include "gui/base/model_view.h"
#include "gui/mainwin.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"
#include <QStackedWidget>

namespace {
qreal safeReal(qreal val) { return qIsFinite(val) ? val : 0.0; }
QString safeRealText(qreal val) { return qIsFinite(val) ? QString::number(val) : ""; }
} // local methods

// ************************************************************************** //
//  local class PeaksModel, used in PeaksView
// ************************************************************************** //

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

QVariant PeaksModel::data(const QModelIndex& index, int role) const {
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


// ************************************************************************** //
//  local class PeaksView
// ************************************************************************** //

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
    connect(gSession, &Session::sigPeakHighlight, this, &PeaksView::onHighlight);
    connect(this, &TableView::clicked, model_, &TableModel::onClicked);
}

// ************************************************************************** //
//  local class RangeControl
// ************************************************************************** //

//! A horizontal row with labels and spin boxes to view and change one peak fit range.

class RangeControl : public QWidget {
public:
    RangeControl();
    void onData();
private:
    CDoubleSpinBox spinRangeMin_{"peakRangeMin", 6, 0., 89.9};
    CDoubleSpinBox spinRangeMax_{"peakRangeMax", 6, 0., 90.};
};

RangeControl::RangeControl() {
    auto hb = new QHBoxLayout();
    setLayout(hb);

    hb->addWidget(new QLabel("range"));
    hb->addWidget(&spinRangeMin_);
    spinRangeMin_.setSingleStep(.1);
    connect(&spinRangeMin_, _SLOT_(QDoubleSpinBox, valueChanged, double),  [this](double val) {
            qreal antival = qMax(spinRangeMax_.value(), val);
            gSession->peaks().selectedPeak()->setRange(Range(val, antival)); });

    hb->addWidget(new QLabel(".."));
    hb->addWidget(&spinRangeMax_);
    spinRangeMax_.setSingleStep(.1);
    connect(&spinRangeMax_, _SLOT_(QDoubleSpinBox, valueChanged, double),  [this](double val) {
            qreal antival = qMin(spinRangeMin_.value(), val);
            gSession->peaks().selectedPeak()->setRange(Range(antival, val)); });
    hb->addWidget(new QLabel("deg"));
    hb->addStretch();

    connect(gSession, &Session::sigPeaks, this, &RangeControl::onData);
    connect(gSession, &Session::sigPeakHighlight, this, &RangeControl::onData);
}

void RangeControl::onData() {
    Peak* peak = gSession->peaks().selectedPeak();
    setEnabled(peak);
    if (!peak)
        return;
    Range range = peak->range();
    spinRangeMin_.setValue(safeReal(range.min));
    spinRangeMax_.setValue(safeReal(range.max));
}


// ************************************************************************** //
//  local class PeakdataView and its dependences
// ************************************************************************** //

//! Virtual base class for RawPeakdataView and FitPeakdataView.

class AnyPeakdataView : public QWidget {
public:
    AnyPeakdataView();
    virtual void updatePeakFun(const PeakFunction&);
protected:
    XLineDisplay readFitPeakX_ {6, true};
    XLineDisplay readFitPeakY_ {6, true};
    XLineDisplay readFitFWHM_ {6, true};
};

AnyPeakdataView::AnyPeakdataView() {
}

void AnyPeakdataView::updatePeakFun(const PeakFunction& peakFun) {
    const qpair& fittedPeak = peakFun.fittedPeak();
    readFitPeakX_.setText(safeRealText(fittedPeak.x));
    readFitPeakY_.setText(safeRealText(fittedPeak.y));
    readFitFWHM_.setText(safeRealText(peakFun.fittedFWHM()));
}

//! Displays outcome of raw data analysis.

class RawPeakdataView : public AnyPeakdataView {
public:
    RawPeakdataView();
};

RawPeakdataView::RawPeakdataView() {
    QGridLayout* lay = new QGridLayout();
    lay->addWidget(new QLabel(""), 1, 1);

    lay->addWidget(new QLabel("centre"), 2, 0);
    lay->addWidget(&readFitPeakX_, 2, 2);
    lay->addWidget(new QLabel("deg"), 2, 3);

    lay->addWidget(new QLabel("fwhm"), 3, 0);
    lay->addWidget(&readFitFWHM_, 3, 2);
    lay->addWidget(new QLabel("deg"), 3, 3);

    lay->addWidget(new QLabel("intens"), 4, 0);
    lay->addWidget(&readFitPeakY_, 4, 2);

    lay->setColumnStretch(4, 1);
    setLayout(lay);
}

//! Displays outcome of peak fit.

class FitPeakdataView : public AnyPeakdataView {
public:
    FitPeakdataView();
    virtual void updatePeakFun(const PeakFunction&);
private:
    XLineDisplay spinGuessPeakX_ {6, true};
    XLineDisplay spinGuessPeakY_ {6, true};
    XLineDisplay spinGuessFWHM_ {6, true};
};

FitPeakdataView::FitPeakdataView() {
    QGridLayout* lay = new QGridLayout();
    lay->addWidget(new QLabel("guess"), 1, 1);
    lay->addWidget(new QLabel("fitted"), 1, 2);

    lay->addWidget(new QLabel("centre"), 2, 0);
    lay->addWidget(&spinGuessPeakX_, 2, 1);
    lay->addWidget(&readFitPeakX_, 2, 2);
    lay->addWidget(new QLabel("deg"), 2, 3);

    lay->addWidget(new QLabel("fwhm"), 3, 0);
    lay->addWidget(&spinGuessFWHM_, 3, 1);
    lay->addWidget(&readFitFWHM_, 3, 2);
    lay->addWidget(new QLabel("deg"), 3, 3);

    lay->addWidget(new QLabel("intens"), 4, 0);
    lay->addWidget(&spinGuessPeakY_, 4, 1);
    lay->addWidget(&readFitPeakY_, 4, 2);

    lay->setColumnStretch(4, 1);
    setLayout(lay);
}

void FitPeakdataView::updatePeakFun(const PeakFunction& peakFun) {
    AnyPeakdataView::updatePeakFun(peakFun);

    const qpair& guessedPeak = peakFun.guessedPeak();
    spinGuessPeakX_.setText(safeRealText(guessedPeak.x));
    spinGuessPeakY_.setText(safeRealText(guessedPeak.y));
    spinGuessFWHM_.setText(safeRealText(peakFun.guessedFWHM()));
}


//! Displays result of either raw data analysis or peak fit.

class PeakdataView : public QStackedWidget {
public:
    PeakdataView();
    void onData();
    void updatePeakFun(const PeakFunction&);
private:
    AnyPeakdataView* widgets_[2];
};

PeakdataView::PeakdataView() {
    addWidget(widgets_[0] = new RawPeakdataView());
    addWidget(widgets_[1] = new FitPeakdataView());
    widgets_[0]->show(); // setCurrentIndex(0);
    connect(gSession, &Session::sigPeaks, this, &PeakdataView::onData);
    connect(gSession, &Session::sigPeakHighlight, this, &PeakdataView::onData);
}

void PeakdataView::onData() {
    Peak* peak = gSession->peaks().selectedPeak();
    setEnabled(peak);
    if (!peak)
        return;
    updatePeakFun(peak->peakFunction());
}

void PeakdataView::updatePeakFun(const PeakFunction& peakFun) {
    int i = peakFun.isRaw() ? 0 : 1;
    widgets_[i]->updatePeakFun(peakFun);
    setCurrentIndex(i);
}


// ************************************************************************** //
//  class ControlsPeakfits
// ************************************************************************** //

ControlsPeakfits::ControlsPeakfits()
    : comboReflType_("reflTyp", FunctionRegistry::instance()->keys()) {

    auto* box = new QVBoxLayout();
    setLayout(box);

    QBoxLayout* hb = new QHBoxLayout();
    box->addLayout(hb);
    hb->addStretch();

    hb->addWidget(new XIconButton(&gGui->triggers->addPeak));
    connect(&gGui->triggers->addPeak, &QAction::triggered, [this]() {
            gSession->peaks().add(comboReflType_.currentText()); });

    hb->addWidget(new XIconButton(&gGui->triggers->removePeak));
    connect(&gGui->triggers->removePeak, &QAction::triggered, [this]() {
            gSession->peaks().remove(); });

    box->addWidget(new PeaksView());

    hb = new QHBoxLayout();
    box->addLayout(hb);

    hb->addWidget(&comboReflType_);
    connect(&comboReflType_, _SLOT_(QComboBox, currentIndexChanged, const QString&),
            [this](const QString& peakFunctionName) {
                if (gSession->peaks().selectedPeak()) { // TODO rm this if
                    gSession->peaks().selectedPeak()->setPeakFunction(peakFunctionName);
                    emit gSession->sigPeaks();
                } });

    hb->addStretch();

    QBoxLayout* vb = new QVBoxLayout();
    box->addLayout(vb);

    vb->addWidget(rangeControl_ = new RangeControl);
    vb->addWidget(peakdataView_ = new PeakdataView);

    update();

    connect(gSession, &Session::sigPeaks, this, &ControlsPeakfits::onPeaks);
}

void ControlsPeakfits::onPeaks() {
    Peak* peak = gSession->peaks().selectedPeak();
    if (peak)
        comboReflType_.setCurrentText(peak->functionName());
};

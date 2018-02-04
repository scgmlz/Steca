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
#include "gui/base/model_view.h"
#include "gui/thehub.h"
#include <QStackedWidget>

namespace {
qreal safeReal(qreal val) { return qIsFinite(val) ? val : 0.0; }
str safeRealText(qreal val) { return qIsFinite(val) ? str::number(val) : ""; }
} // local methods

// ************************************************************************** //
//  local class PeaksModel, used in PeaksView
// ************************************************************************** //

//! Model for PeaksView.

class PeaksModel : public TableModel {
public:
    PeaksModel() : TableModel() {}

    void addPeak(const QString& functionName);
    void removePeak();

    int columnCount() const final { return NUM_COLUMNS; }
    int rowCount() const final { return gSession->peaks().count(); }
    QVariant data(const QModelIndex&, int) const;

    enum { COL_ID = 1, COL_TYPE, NUM_COLUMNS };
};

void PeaksModel::addPeak(const QString& functionName) {
    gSession->peaks().add(functionName);
}

void PeaksModel::removePeak() {
    gSession->peaks().remove();
}

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
            return str::number(row + 1);
        case COL_TYPE:
            return peak.functionName();
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
        if (row==gSession->peaks().selectedIndex())
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

class PeaksView final : public ListView {
public:
    PeaksView();

    void addPeak(const QString&);
    void removeSelected();
    void update();

private:
    void currentChanged(QModelIndex const&, QModelIndex const&) override final;
    PeaksModel* model_;
};

PeaksView::PeaksView() : ListView() {
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    model_ = new PeaksModel();
    setModel(model_);
    for_i (model_->columnCount())
        resizeColumnToContents(i);
}

void PeaksView::addPeak(const QString& functionName) {
    model_->addPeak(functionName);
    update();
}

void PeaksView::removeSelected() {
    model_->removePeak();
    update();
}

void PeaksView::update() {
    model_->resetModel();
}

//! Overrides QAbstractItemView. This slot is called when a new item becomes the current item.
void PeaksView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    gSession->peaks().select(current.row());
    update();
}


// ************************************************************************** //
//  local class RangeControl
// ************************************************************************** //

//! A horizontal row with labels and spin boxes to view and change one peak fit range.

class RangeControl : public QWidget {
public:
    RangeControl();
    void update();
private:
    QDoubleSpinBox *spinRangeMin_, *spinRangeMax_;
};

RangeControl::RangeControl() {

    auto hb = newQ::HBoxLayout();
    setLayout(hb);

    hb->addWidget(newQ::Label("", "range"));
    hb->addWidget((spinRangeMin_ = newQ::DoubleSpinBox("spinRangeMin_", 6, 0., 89.9)));
    spinRangeMin_->setSingleStep(.1);
    connect(spinRangeMin_, _SLOT_(QDoubleSpinBox, valueChanged, double),  [this](double val) {
            qreal antival = qMax(spinRangeMax_->value(), val);
            gSession->peaks().selectedPeak()->setRange(Range(val, antival)); });

    hb->addWidget(newQ::Label("", ".."));
    hb->addWidget((spinRangeMax_ = newQ::DoubleSpinBox("spinRangeMax_", 6, 0., 90.)));
    spinRangeMax_->setSingleStep(.1);
    connect(spinRangeMax_, _SLOT_(QDoubleSpinBox, valueChanged, double),  [this](double val) {
            qreal antival = qMin(spinRangeMin_->value(), val);
            gSession->peaks().selectedPeak()->setRange(Range(antival, val)); });
    hb->addWidget(newQ::Label("", "deg"));
    hb->addStretch();
}

void RangeControl::update() {
    Range range = gSession->peaks().selectedPeak()->range();
    spinRangeMin_->setValue(safeReal(range.min));
    spinRangeMax_->setValue(safeReal(range.max));
    setEnabled(true);
}


// ************************************************************************** //
//  local class PeakdataView and its dependences
// ************************************************************************** //

//! Virtual base class for RawPeakdataView and FitPeakdataView.

class AnyPeakdataView : public QWidget {
public:
    AnyPeakdataView();
    virtual void update(const PeakFunction&);
protected:
    QLineEdit *readFitPeakX_, *readFitPeakY_, *readFitFWHM_;
};

AnyPeakdataView::AnyPeakdataView() {
    readFitPeakX_ = newQ::LineDisplay("readFitPeakX_", 6, true);
    readFitFWHM_ = newQ::LineDisplay("readFitFWHM_", 6, true);
    readFitPeakY_ = newQ::LineDisplay("readFitPeakY_", 6, true);
}

void AnyPeakdataView::update(const PeakFunction& peakFun) {
    const qpair& fittedPeak = peakFun.fittedPeak();
    readFitPeakX_->setText(safeRealText(fittedPeak.x));
    readFitPeakY_->setText(safeRealText(fittedPeak.y));
    readFitFWHM_->setText(safeRealText(peakFun.fittedFWHM()));
}

//! Displays outcome of raw data analysis.

class RawPeakdataView : public AnyPeakdataView {
public:
    RawPeakdataView();
};

RawPeakdataView::RawPeakdataView() {
    QGridLayout* lay = newQ::GridLayout();
    lay->addWidget(newQ::Label("", ""), 1, 1);

    lay->addWidget(newQ::Label("", "centre"), 2, 0);
    lay->addWidget(readFitPeakX_, 2, 2);
    lay->addWidget(newQ::Label("", "deg"), 2, 3);

    lay->addWidget(newQ::Label("", "fwhm"), 3, 0);
    lay->addWidget(readFitFWHM_, 3, 2);
    lay->addWidget(newQ::Label("", "deg"), 3, 3);

    lay->addWidget(newQ::Label("", "intens"), 4, 0);
    lay->addWidget(readFitPeakY_, 4, 2);

    lay->setColumnStretch(4, 1);
    setLayout(lay);
}

//! Displays outcome of peak fit.

class FitPeakdataView : public AnyPeakdataView {
public:
    FitPeakdataView();
    virtual void update(const PeakFunction&);
private:
    QLineEdit *spinGuessPeakX_, *spinGuessPeakY_, *spinGuessFWHM_;
};

FitPeakdataView::FitPeakdataView() {
    spinGuessPeakX_ = newQ::LineDisplay("spinGuessPeakX_", 6, true);
    spinGuessFWHM_ = newQ::LineDisplay("spinGuessFWHM_", 6, true);
    spinGuessPeakY_ = newQ::LineDisplay("spinGuessPeakY_", 6, true);

    QGridLayout* lay = newQ::GridLayout();
    lay->addWidget(newQ::Label("", "guess"), 1, 1);
    lay->addWidget(newQ::Label("", "fitted"), 1, 2);

    lay->addWidget(newQ::Label("", "centre"), 2, 0);
    lay->addWidget(spinGuessPeakX_, 2, 1);
    lay->addWidget(readFitPeakX_, 2, 2);
    lay->addWidget(newQ::Label("", "deg"), 2, 3);

    lay->addWidget(newQ::Label("", "fwhm"), 3, 0);
    lay->addWidget(spinGuessFWHM_, 3, 1);
    lay->addWidget(readFitFWHM_, 3, 2);
    lay->addWidget(newQ::Label("", "deg"), 3, 3);

    lay->addWidget(newQ::Label("", "intens"), 4, 0);
    lay->addWidget(spinGuessPeakY_, 4, 1);
    lay->addWidget(readFitPeakY_, 4, 2);

    lay->setColumnStretch(4, 1);
    setLayout(lay);
}

void FitPeakdataView::update(const PeakFunction& peakFun) {
    AnyPeakdataView::update(peakFun);

    const qpair& guessedPeak = peakFun.guessedPeak();
    spinGuessPeakX_->setText(safeRealText(guessedPeak.x));
    spinGuessPeakY_->setText(safeRealText(guessedPeak.y));
    spinGuessFWHM_->setText(safeRealText(peakFun.guessedFWHM()));
}


//! Displays result of either raw data analysis or peak fit.

class PeakdataView : public QStackedWidget {
public:
    PeakdataView();
    void update(const PeakFunction&);
private:
    AnyPeakdataView* widgets_[2];
};

PeakdataView::PeakdataView() {
    addWidget(widgets_[0] = new RawPeakdataView());
    addWidget(widgets_[1] = new FitPeakdataView());
    widgets_[0]->show(); // setCurrentIndex(0);
}

void PeakdataView::update(const PeakFunction& peakFun) {
    int i = peakFun.isRaw() ? 0 : 1;
    widgets_[i]->update(peakFun);
    setCurrentIndex(i);
}


// ************************************************************************** //
//  class ControlsPeakfits
// ************************************************************************** //

ControlsPeakfits::ControlsPeakfits() {

    auto* box = newQ::VBoxLayout();
    setLayout(box);

    QBoxLayout* hb = newQ::HBoxLayout();
    box->addLayout(hb);

    hb->addWidget(newQ::IconButton(gHub->toggle_selRegions));
    hb->addWidget(newQ::IconButton(gHub->toggle_showBackground));
    hb->addStretch();

    hb->addWidget(newQ::IconButton(gHub->trigger_addPeak));
    connect(gHub->trigger_addPeak, &QAction::triggered, [this]() {
            peaksView_->addPeak(comboReflType_->currentText());
            update();
        });


    hb->addWidget(newQ::IconButton(gHub->trigger_removePeak));
    connect(gHub->trigger_removePeak, &QAction::triggered, [this]() {
            peaksView_->removeSelected();
            update();
        });

    box->addWidget((peaksView_ = new PeaksView()));

    hb = newQ::HBoxLayout();
    box->addLayout(hb);

    comboReflType_ = new QComboBox;
    comboReflType_->addItems(FunctionRegistry::instance()->keys());
    hb->addWidget(comboReflType_);
    connect(comboReflType_, _SLOT_(QComboBox, currentIndexChanged, const QString&),
            [this](const QString& peakFunctionName) {
                if (gSession->peaks().selectedPeak()) { // TODO rm this if
                    gSession->peaks().selectedPeak()->setPeakFunction(peakFunctionName);
                    emit gSession->sigPeaks();
                } });

    hb->addStretch();

    QBoxLayout* vb = newQ::VBoxLayout();
    box->addLayout(vb);

    vb->addWidget(rangeControl_ = new RangeControl);
    vb->addWidget(peakdataView_ = new PeakdataView);

    update();

    connect(gSession, &Session::sigPeaks, this, &ControlsPeakfits::onPeaks);
}

void ControlsPeakfits::onPeaks() {
    peaksView_->update();
    Peak* peak = gSession->peaks().selectedPeak();
    silentSpin_ = true;

    if (!peak) {
        // do not set comboReflType - we want it to stay as it is
        rangeControl_->setEnabled(false);
        peakdataView_->setEnabled(false);

    } else {
        {
            QSignalBlocker __(comboReflType_);
            comboReflType_->setCurrentText(peak->functionName());
        }
        rangeControl_->update();
        peakdataView_->update(peak->peakFunction());
    }

    silentSpin_ = false;
};

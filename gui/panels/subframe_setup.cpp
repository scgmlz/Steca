// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_setup.cpp
//! @brief     Implements class SubframeSetup, and local classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/panels/subframe_setup.h"
#include "core/session.h"
#include "gui/base/table_model.h"
#include "gui/thehub.h"
#include "gui/base/tree_views.h" // inheriting from
#include "gui/base/new_q.h"

namespace {
qreal safeReal(qreal val) { return qIsFinite(val) ? val : 0.0; }
str safeRealText(qreal val) { return qIsFinite(val) ? str::number(val) : ""; }
} // local methods

// ************************************************************************** //
//  local class PeaksModel, used in PeaksView
// ************************************************************************** //

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

class PeaksView final : public ListView {
public:
    PeaksView();

    void clear();
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

void PeaksView::clear() {
    gSession->peaks().clear();
    update();
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
    model_->signalReset();
}

//! Overrides QAbstractItemView. This slot is called when a new item becomes the current item.
void PeaksView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    gSession->peaks().select(current.row());
    update();
}

// ************************************************************************** //
//  class RangeControl
// ************************************************************************** //

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

    hb->addWidget(newQ::Label("range"));
    hb->addWidget((spinRangeMin_ = newQ::DoubleSpinBox(6, 0., 89.9)));
    spinRangeMin_->setSingleStep(.1);
    connect(spinRangeMin_, _SLOT_(QDoubleSpinBox, valueChanged, double),  [this](double val) {
            if (val>=spinRangeMax_->value())
                spinRangeMax_->setValue(val+0.1);
            gSession->peaks().selectedPeak()->setRange(Range(val, spinRangeMax_->value())); });

    hb->addWidget(newQ::Label(".."));
    hb->addWidget((spinRangeMax_ = newQ::DoubleSpinBox(6, 0.1, 90.)));
    spinRangeMax_->setSingleStep(.1);
    connect(spinRangeMax_, _SLOT_(QDoubleSpinBox, valueChanged, double),  [this](double val) {
            if (val<=spinRangeMin_->value())
                spinRangeMin_->setValue(val-0.1);
            gSession->peaks().selectedPeak()->setRange(Range(spinRangeMin_->value(), val)); });
    hb->addWidget(newQ::Label("deg"));
    hb->addStretch();
}

void RangeControl::update() {
    const Range& range = gSession->peaks().selectedPeak()->range();
    spinRangeMin_->setValue(safeReal(range.min));
    spinRangeMax_->setValue(safeReal(range.max));
}

// ************************************************************************** //
//  class ControlsPeakfits
// ************************************************************************** //

//! A widget with controls to view and change the detector geometry.

class ControlsPeakfits : public QWidget {
public:
    ControlsPeakfits();
private:
    void onPeaks();

    PeaksView* peaksView_;
    RangeControl rangeControl_;
    QComboBox* comboReflType_;
    QLineEdit *spinGuessPeakX_, *spinGuessPeakY_, *spinGuessFWHM_;
    QLineEdit *readFitPeakX_, *readFitPeakY_, *readFitFWHM_;

    bool silentSpin_ = false;
};

ControlsPeakfits::ControlsPeakfits() {

    auto* box = newQ::VBoxLayout();
    setLayout(box);

    QBoxLayout* hb = newQ::HBoxLayout();
    box->addLayout(hb);

    hb->addWidget(newQ::IconButton(gHub->toggle_selRegions));
    hb->addWidget(newQ::IconButton(gHub->toggle_showBackground));

    hb->addWidget(newQ::IconButton(gHub->trigger_clearPeaks));
    connect(gHub->trigger_clearPeaks, &QAction::triggered, [this]() {
            peaksView_->clear();
            update();
        });

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

    vb->addWidget(&rangeControl_);

    QGridLayout* gb = newQ::GridLayout();
    vb->addLayout(gb);

    gb->addWidget(newQ::Label("guess"), 1, 1);
    gb->addWidget(newQ::Label("fitted"), 1, 2);

    gb->addWidget(newQ::Label("centre"), 2, 0);
    gb->addWidget((spinGuessPeakX_ = newQ::LineDisplay(6, true)), 2, 1);
    gb->addWidget((readFitPeakX_ = newQ::LineDisplay(6, true)), 2, 2);

    gb->addWidget(newQ::Label("fwhm"), 3, 0);
    gb->addWidget((spinGuessFWHM_ = newQ::LineDisplay(6, true)), 3, 1);
    gb->addWidget((readFitFWHM_ = newQ::LineDisplay(6, true)), 3, 2);

    gb->addWidget(newQ::Label("intens"), 4, 0);
    gb->addWidget((spinGuessPeakY_ = newQ::LineDisplay(6, true)), 4, 1);
    gb->addWidget((readFitPeakY_ = newQ::LineDisplay(6, true)), 4, 2);

    gb->setColumnStretch(4, 1);

    update();

    connect(gSession, &Session::sigPeaks, this, &ControlsPeakfits::onPeaks);
}

void ControlsPeakfits::onPeaks() {
    peaksView_->update();
    bool on = gSession->peaks().count();
    qDebug() << "onPeaks: " << on;
    readFitPeakX_->setEnabled(on);
    readFitPeakY_->setEnabled(on);
    readFitFWHM_->setEnabled(on);

    Peak* peak = gSession->peaks().selectedPeak();
    silentSpin_ = true;

    if (!peak) {
        rangeControl_.hide();
        // do not set comboReflType - we want it to stay as it is

        /*
        spinGuessPeakX_->setValue(0);
        spinGuessPeakY_->setValue(0);
        spinGuessFWHM_->setValue(0);
        */
        readFitPeakX_->clear();
        readFitPeakY_->clear();
        readFitFWHM_->clear();
    } else {
        {
            QSignalBlocker __(comboReflType_);
            comboReflType_->setCurrentText(peak->functionName());
        }

        rangeControl_.show();
        rangeControl_.update();

        const PeakFunction& peakFun = peak->peakFunction();
        const qpair& guessedPeak = peakFun.guessedPeak();
        spinGuessPeakX_->setText(safeRealText(guessedPeak.x));
        spinGuessPeakY_->setText(safeRealText(guessedPeak.y));
        spinGuessFWHM_->setText(safeRealText(peakFun.guessedFWHM()));

        const qpair& fittedPeak = peakFun.fittedPeak();
        readFitPeakX_->setText(safeRealText(fittedPeak.x));
        readFitPeakY_->setText(safeRealText(fittedPeak.y));
        readFitFWHM_->setText(safeRealText(peakFun.fittedFWHM()));
    }

    silentSpin_ = false;
};


// ************************************************************************** //
//  class ControlsDetector
// ************************************************************************** //

//! A widget with controls to view and change the detector geometry.

class ControlsDetector : public QWidget {
public:
    ControlsDetector();
private:
    void toSession();
    void fromSession();

    QDoubleSpinBox *detDistance_, *detPixelSize_;
    QSpinBox *beamOffsetI_, *beamOffsetJ_;
    QSpinBox *cutLeft_, *cutTop_, *cutRight_, *cutBottom_;
};

ControlsDetector::ControlsDetector() {

    auto* box = newQ::VBoxLayout();
    setLayout(box);

    connect(gSession, &Session::sigDetector, this, &ControlsDetector::fromSession);

    // widgets

    detDistance_ = newQ::DoubleSpinBox(6, true, Geometry::MIN_DETECTOR_DISTANCE);
    detPixelSize_ = newQ::DoubleSpinBox(6, true, Geometry::MIN_DETECTOR_PIXEL_SIZE);
    detPixelSize_->setDecimals(3);

    detDistance_->setValue(Geometry::DEF_DETECTOR_DISTANCE);
    detPixelSize_->setValue(Geometry::DEF_DETECTOR_PIXEL_SIZE);

    connect(detDistance_, _SLOT_(QDoubleSpinBox, valueChanged, double),
            [this]() { toSession(); });
    connect(detPixelSize_, _SLOT_(QDoubleSpinBox, valueChanged, double),
            [this]() { toSession(); });

    beamOffsetI_ = newQ::SpinBox(6, true);
    beamOffsetJ_ = newQ::SpinBox(6, true);

//    connect(beamOffsetI_, _SLOT_(QSpinBox, valueChanged, int), [this]() { setToHub(); });

//    connect(beamOffsetJ_, _SLOT_(QSpinBox, valueChanged, int), [this]() { setToHub(); });

    cutLeft_ = newQ::SpinBox(4, false, 0);
    cutTop_ = newQ::SpinBox(4, false, 0);
    cutRight_ = newQ::SpinBox(4, false, 0);
    cutBottom_ = newQ::SpinBox(4, false, 0);

    auto _setImageCut = [this](bool isTopOrLeft, int value) {
        debug::ensure(value >= 0);
        if (gHub->toggle_linkCuts->isChecked())
            gSession->setImageCut(isTopOrLeft, true,
                                  ImageCut(value, value, value, value));
        else
            gSession->setImageCut(isTopOrLeft, false,
                                  ImageCut(cutLeft_->value(), cutTop_->value(),
                                           cutRight_->value(), cutBottom_->value()));
    };

    connect(cutLeft_, _SLOT_(QSpinBox, valueChanged, int), [_setImageCut](int value) {
            _setImageCut(true, value);
        });

    connect(cutTop_, _SLOT_(QSpinBox, valueChanged, int), [_setImageCut](int value) {
            _setImageCut(true, value);
        });

    connect(cutRight_, _SLOT_(QSpinBox, valueChanged, int), [_setImageCut](int value) {
            _setImageCut(false, value);
        });

    connect(cutBottom_, _SLOT_(QSpinBox, valueChanged, int), [_setImageCut](int value) {
            _setImageCut(false, value);
        });

    // layout

    QGridLayout* grid = newQ::GridLayout();
    int row = 0;

    auto _add = [&grid, &row](const QVector<QWidget*>& ws, int left = 1) {
        int i = 0, cnt = ws.count();
        QBoxLayout* box = newQ::HBoxLayout();
        box->addStretch(1);
        while (i < left)
            box->addWidget(ws.at(i++));
        grid->addLayout(box, row, 0);
        box = newQ::HBoxLayout();
        while (i < cnt)
            box->addWidget(ws.at(i++));
        grid->addLayout(box, row, 1);
        box->addStretch(1);
        row++;
    };

    _add({ newQ::Label("det. distance"),
                detDistance_,
                newQ::Label("mm") });
    _add({ newQ::Label("pixel size"),
                detPixelSize_,
                newQ::Label("mm") });
    _add({ newQ::Label("beam offset X"),
                beamOffsetI_,
                newQ::Label("pix") });
    _add({ newQ::Label("Y"),
                beamOffsetJ_,
                newQ::Label("pix") });
    _add({ newQ::Label("image rotate"),
                newQ::IconButton(gHub->trigger_rotateImage),
                newQ::Label("mirror"),
                newQ::IconButton(gHub->toggle_mirrorImage) });
    _add({ newQ::IconButton(gHub->toggle_linkCuts),
                newQ::Label("cut"),
                newQ::Icon(":/icon/cutLeft"),
                cutLeft_,
                newQ::Icon(":/icon/cutRight"),
                cutRight_ }, 3);
    _add({ newQ::Icon(":/icon/cutTop"),
                cutTop_,
                newQ::Icon(":/icon/cutBottom"),
                cutBottom_ });

    grid->setColumnStretch(grid->columnCount(), 1);

    box->addLayout(grid);
    box->addStretch();
}

void ControlsDetector::toSession() {
    gSession->setGeometry(
        qMax(qreal(Geometry::MIN_DETECTOR_DISTANCE), detDistance_->value()),
        qMax(qreal(Geometry::MIN_DETECTOR_PIXEL_SIZE), detPixelSize_->value()),
        IJ(beamOffsetI_->value(), beamOffsetJ_->value()));
}

void ControlsDetector::fromSession() {
    const Geometry& g = gSession->geometry();

    detDistance_->setValue(g.detectorDistance);
    detPixelSize_->setValue(g.pixSize);

    beamOffsetI_->setValue(g.midPixOffset.i);
    beamOffsetJ_->setValue(g.midPixOffset.j);

    const ImageCut& cut = gSession->imageCut();

    cutLeft_->setValue(cut.left);
    cutTop_->setValue(cut.top);
    cutRight_->setValue(cut.right);
    cutBottom_->setValue(cut.bottom);
}


// ************************************************************************** //
//  class ControlsBaseline
// ************************************************************************** //

//! A widget with controls to change the baseline fitting.

class ControlsBaseline : public QWidget {
public:
    ControlsBaseline();
private:
    QSpinBox* spinDegree_;
};

ControlsBaseline::ControlsBaseline() {
    auto* box = newQ::VBoxLayout();
    setLayout(box);

    QBoxLayout* hb = newQ::HBoxLayout();
    box->addLayout(hb);
    hb->addWidget(newQ::IconButton(gHub->toggle_selRegions));
    hb->addWidget(newQ::IconButton(gHub->toggle_showBackground));
    hb->addWidget(newQ::IconButton(gHub->trigger_clearBackground));
    hb->addWidget(newQ::Label("Pol. degree:"));
    hb->addWidget((spinDegree_ = newQ::SpinBox(4, false, 0, TheHub::MAX_POLYNOM_DEGREE)));
    connect(spinDegree_, _SLOT_(QSpinBox, valueChanged, int), [this](int degree) {
            gSession->baseline().setPolynomDegree(degree); });
    connect(gSession, &Session::sigBaseline, [this]() {
            spinDegree_->setValue(gSession->baseline().polynomDegree()); });
    hb->addStretch();

    box->addStretch(1);
}


// ************************************************************************** //
//  class SubframeSetup
// ************************************************************************** //

SubframeSetup::SubframeSetup() {
    setTabPosition(QTabWidget::North);

    addTab(new ControlsDetector(), "Detector");
    addTab(new ControlsBaseline(), "Baseline");
    addTab(new ControlsPeakfits(), "Peakfits");

    connect(this, &SubframeSetup::currentChanged, [this](int index) {
        eFittingTab tab;
        if (index==1)
            tab = eFittingTab::BACKGROUND;
        else if (index==2)
            tab = eFittingTab::REFLECTIONS;
        else
            tab = eFittingTab::NONE;
        gHub->setFittingTab(tab);
    });

    gHub->setFittingTab(eFittingTab::NONE);
}

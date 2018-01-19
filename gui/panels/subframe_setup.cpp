// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_setup.cpp
//! @brief     Implements class SubframeSetup
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
#include "gui/base/various_widgets.h"

static qreal safeReal(qreal val) { return qIsFinite(val) ? val : 0.0; }
static str safeRealText(qreal val) { return qIsFinite(val) ? str::number(val) : ""; }


// ************************************************************************** //
//  local class ReflectionsModel
// ************************************************************************** //

class ReflectionsModel : public TableModel {
public:
    ReflectionsModel() : TableModel() {}

    void addReflection(const QString& peakFunctionName) { gHub->addReflection(peakFunctionName); }
    void removeReflection(int i) { gHub->removeReflection(i); }

    int columnCount() const final { return NUM_COLUMNS; }
    int rowCount() const final { return gSession->reflections().count(); }
    str displayData(int row, int col) const;
    str displayData(int row) const;
    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_ID = 1, COL_TYPE, NUM_COLUMNS };
};


str ReflectionsModel::displayData(int row, int col) const {
    switch (col) {
    case COL_ID:
        return str::number(row + 1);
    case COL_TYPE:
        return gSession->reflections().at(row)->peakFunction().name();
    default:
        NEVER return "";
    }
}

str ReflectionsModel::displayData(int row) const {
    return displayData(row, COL_ID) + ": " + displayData(row, COL_TYPE);
}

QVariant ReflectionsModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || rowCount() <= row)
        return {};
    switch (role) {
    case Qt::DisplayRole: {
        int col = index.column();
        if (col < 1)
            return {};
        switch (col) {
        case COL_ID:
        case COL_TYPE:
            return displayData(row, col);
        default:
            return {};
        }
    }
    case Qt::UserRole:
        return QVariant::fromValue<shp_Reflection>(gSession->reflections().at(row));
    default:
        return {};
    }
}

QVariant ReflectionsModel::headerData(int col, Qt::Orientation, int role) const {
    if (Qt::DisplayRole == role && COL_ID == col)
        return "#";
    return {};
}


// ************************************************************************** //
//  local class ReflectionView
// ************************************************************************** //

class ReflectionView final : public ListView {
public:
    ReflectionView();

    void addReflection(const QString& peakFunctionName);
    void removeSelected();
    void updateSingleSelection();
    void clear();

    bool hasReflections() const { return model()->rowCount() > 0; }
    shp_Reflection selectedReflection() const;

private:
    void selectionChanged(QItemSelection const&, QItemSelection const&);
    ReflectionsModel* model() const { return static_cast<ReflectionsModel*>(ListView::model()); }
};

ReflectionView::ReflectionView() : ListView() {
    auto reflectionsModel = new ReflectionsModel();
    setModel(reflectionsModel);
    debug::ensure(dynamic_cast<ReflectionsModel*>(ListView::model()));

    for_i (model()->columnCount())
        resizeColumnToContents(i);
}

void ReflectionView::addReflection(const QString& peakFunctionName) {
    model()->addReflection(peakFunctionName);
    updateSingleSelection();
}

void ReflectionView::removeSelected() {
    int row = currentIndex().row();
    if (row < 0 || model()->rowCount() <= row)
        return;
    model()->removeReflection(row);
    updateSingleSelection();
}

void ReflectionView::clear() {
    for (int row = model()->rowCount(); row-- > 0;) {
        model()->removeReflection(row);
        updateSingleSelection();
    }
}

shp_Reflection ReflectionView::selectedReflection() const {
    QList<QModelIndex> indexes = selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
        return shp_Reflection();
    return model()->data(indexes.first(), Qt::UserRole).value<shp_Reflection>();
}

void ReflectionView::updateSingleSelection() {
    ListView::updateSingleSelection();
    gHub->trigger_removeReflection->setEnabled(hasReflections());
}

void ReflectionView::selectionChanged(
    QItemSelection const& selected, QItemSelection const& deselected) {
    ListView::selectionChanged(selected, deselected);

    QList<QModelIndex> indexes = selected.indexes();
    gHub->tellSelectedReflection(
        indexes.isEmpty() ?
        shp_Reflection() :
        model()->data(indexes.first(), Qt::UserRole).value<shp_Reflection>());
}


// ************************************************************************** //
//  class SubframeSetup
// ************************************************************************** //

SubframeSetup::SubframeSetup() {
    setTabPosition(QTabWidget::North);

    int backgroundTabIndex, reflectionTabIndex;

    // ==== image geometry tab ====
    {
        QBoxLayout& box = newQ::Tab(this, "Geometry")->box();

        connect(gHub, &TheHub::sigGeometryChanged, [this](){ setFromHub(); });

        // widgets

        detDistance_ = newQ::DoubleSpinBox(4, 2, Geometry::MIN_DETECTOR_DISTANCE);
        detPixelSize_ = newQ::DoubleSpinBox(4, 2, Geometry::MIN_DETECTOR_PIXEL_SIZE);
        detPixelSize_->setDecimals(3);

        detDistance_->setValue(Geometry::DEF_DETECTOR_DISTANCE);
        detPixelSize_->setValue(Geometry::DEF_DETECTOR_PIXEL_SIZE);

        connect(detDistance_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this]() { setToHub(); });

        connect(
            detPixelSize_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this]() { setToHub(); });

        beamOffsetI_ = newQ::SpinBox(6, true);
        beamOffsetJ_ = newQ::SpinBox(6, true);

        connect(beamOffsetI_, _SLOT_(QSpinBox, valueChanged, int), [this]() { setToHub(); });

        connect(beamOffsetJ_, _SLOT_(QSpinBox, valueChanged, int), [this]() { setToHub(); });

        cutLeft_ = newQ::SpinBox(4, false, 0);
        cutTop_ = newQ::SpinBox(4, false, 0);
        cutRight_ = newQ::SpinBox(4, false, 0);
        cutBottom_ = newQ::SpinBox(4, false, 0);

        auto _setImageCut = [this](bool isTopOrLeft, int value) {
            debug::ensure(value >= 0);
            if (gHub->toggle_linkCuts->isChecked())
                gHub->setImageCut(
                    isTopOrLeft, true,
                    ImageCut(value, value, value, value));
            else
                gHub->setImageCut(
                    isTopOrLeft, false, ImageCut(
                                        cutLeft_->value(), cutTop_->value(),
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

        box.addLayout(grid);
        box.addStretch();
    }

    // ==== background fit tab ====
    {
        backgroundTabIndex = count();
        QBoxLayout& box = newQ::Tab(this, "Background")->box();
        QBoxLayout* hb = newQ::HBoxLayout();
        box.addLayout(hb);

        hb->addWidget(newQ::IconButton(gHub->toggle_selRegions));
        hb->addWidget(newQ::IconButton(gHub->toggle_showBackground));
        hb->addWidget(newQ::IconButton(gHub->trigger_clearBackground));
        hb->addWidget(newQ::Label("Pol. degree:"));
        hb->addWidget((spinDegree_ =
                       newQ::SpinBox(4, false, 0, TheHub::MAX_POLYNOM_DEGREE)));
        hb->addStretch();

        box.addStretch(1);

        connect(spinDegree_, _SLOT_(QSpinBox, valueChanged, int), [this](int degree) {
                debug::ensure(degree >= 0);
                gHub->setBgPolyDegree(degree);
            });

        connect(gHub, &TheHub::sigBgChanged, [this](){
                spinDegree_->setValue(gSession->bgPolyDegree()); });
    }

    // ==== peak fits tab ====
    {
        reflectionTabIndex = count();
        QBoxLayout& box = newQ::Tab(this, "Reflections")->box();
        QBoxLayout* hb = newQ::HBoxLayout();
        box.addLayout(hb);

        hb->addWidget(newQ::IconButton(gHub->toggle_selRegions));
        hb->addWidget(newQ::IconButton(gHub->toggle_showBackground));
        hb->addWidget(newQ::IconButton(gHub->trigger_clearReflections));
        hb->addStretch();

        box.addWidget((reflectionView_ = new ReflectionView()));

        hb = newQ::HBoxLayout();
        box.addLayout(hb);

        comboReflType_ = new QComboBox;
        comboReflType_->addItems(FunctionRegistry::instance()->keys());
        hb->addWidget(comboReflType_);
        hb->addStretch();
        hb->addWidget(newQ::IconButton(gHub->trigger_addReflection));
        hb->addWidget(newQ::IconButton(gHub->trigger_removeReflection));

        QBoxLayout* vb = newQ::VBoxLayout();
        box.addLayout(vb);

        QGridLayout* gb = newQ::GridLayout();
        vb->addLayout(gb);

        gb->addWidget(newQ::Label("min"), 0, 0);
        gb->addWidget((spinRangeMin_ = newQ::DoubleSpinBox(4, 2, .0)), 0, 1);
        spinRangeMin_->setSingleStep(.1);
        gb->addWidget(newQ::Label("max"), 0, 2);
        gb->addWidget((spinRangeMax_ = newQ::DoubleSpinBox(4, 2, .0)), 0, 3);
        spinRangeMax_->setSingleStep(.1);

        gb->addWidget(newQ::Label("guess x"), 1, 0);
        gb->addWidget((spinGuessPeakX_ = newQ::DoubleSpinBox(4, 2, .0)), 1, 1);
        spinGuessPeakX_->setSingleStep(.1);
        gb->addWidget(newQ::Label("y"), 1, 2);
        gb->addWidget((spinGuessPeakY_ = newQ::DoubleSpinBox(4, 2, .0)), 1, 3);
        spinGuessPeakY_->setSingleStep(.1);

        gb->addWidget(newQ::Label("fwhm"), 2, 0);
        gb->addWidget((spinGuessFWHM_ = newQ::DoubleSpinBox(4, 2, .0)), 2, 1);
        spinGuessFWHM_->setSingleStep(.1);

        gb->addWidget(newQ::Label("fit x"), 3, 0);
        gb->addWidget((readFitPeakX_ = newQ::LineDisplay(6, true)), 3, 1);
        gb->addWidget(newQ::Label("y"), 3, 2);
        gb->addWidget((readFitPeakY_ = newQ::LineDisplay(6, true)), 3, 3);

        gb->addWidget(newQ::Label("fwhm"), 4, 0);
        gb->addWidget((readFitFWHM_ = newQ::LineDisplay(6, true)), 4, 1);

        gb->setColumnStretch(4, 1);

        auto _updateReflectionControls = [this]() {
            bool on = reflectionView_->hasReflections();
            spinRangeMin_->setEnabled(on);
            spinRangeMax_->setEnabled(on);
            spinGuessPeakX_->setEnabled(on);
            spinGuessPeakY_->setEnabled(on);
            spinGuessFWHM_->setEnabled(on);
            readFitPeakX_->setEnabled(on);
            readFitPeakY_->setEnabled(on);
            readFitFWHM_->setEnabled(on);
        };

        _updateReflectionControls();

        connect(gHub->trigger_addReflection, &QAction::triggered,
                [this, _updateReflectionControls]() {
            reflectionView_->addReflection(comboReflType_->currentText());
            _updateReflectionControls();
        });

        connect(gHub->trigger_removeReflection, &QAction::triggered,
                [this, _updateReflectionControls]() {
            reflectionView_->removeSelected();
            _updateReflectionControls();
        });

        connect(gHub->trigger_clearReflections, &QAction::triggered,
                [this, _updateReflectionControls]() {
            reflectionView_->clear();
            _updateReflectionControls();
        });

        connect(gHub, &TheHub::sigReflectionsChanged,
                [this, _updateReflectionControls]() {
                    reflectionView_->updateSingleSelection();
                    _updateReflectionControls(); }
            );

        connect(comboReflType_, _SLOT_(QComboBox, currentIndexChanged, const QString&),
                [this](const QString& peakFunctionName) {
            gHub->setPeakFunction(peakFunctionName);
        });

        auto _setReflControls = [this](shp_Reflection reflection) {
            silentSpin_ = true;

            if (reflection.isNull()) {
                // do not set comboReflType - we want it to stay as it is
                spinRangeMin_->setValue(0);
                spinRangeMax_->setValue(0);
                spinGuessPeakX_->setValue(0);
                spinGuessPeakY_->setValue(0);
                spinGuessFWHM_->setValue(0);
                readFitPeakX_->clear();
                readFitPeakY_->clear();
                readFitFWHM_->clear();
            } else {
                {
                    QSignalBlocker __(comboReflType_);
                    comboReflType_->setCurrentText(reflection->peakFunctionName());
                }

                const Range& range = reflection->range();
                spinRangeMin_->setValue(safeReal(range.min));
                spinRangeMax_->setValue(safeReal(range.max));

                const PeakFunction& peakFun = reflection->peakFunction();
                const qpair& guessedPeak = peakFun.guessedPeak();
                spinGuessPeakX_->setValue(safeReal(guessedPeak.x));
                spinGuessPeakY_->setValue(safeReal(guessedPeak.y));
                spinGuessFWHM_->setValue(safeReal(peakFun.guessedFWHM()));

                const qpair& fittedPeak = peakFun.fittedPeak();
                readFitPeakX_->setText(safeRealText(fittedPeak.x));
                readFitPeakY_->setText(safeRealText(fittedPeak.y));
                readFitFWHM_->setText(safeRealText(peakFun.fittedFWHM()));
            }

            silentSpin_ = false;
        };

        connect(gHub, &TheHub::sigReflectionSelected,
            [_setReflControls](shp_Reflection reflection) { _setReflControls(reflection); });

        connect(gHub, &TheHub::sigReflectionData,
            [_setReflControls](shp_Reflection reflection) { _setReflControls(reflection); });

        auto _newReflData = [this](bool invalidateGuesses) {
            if (!silentSpin_) {
                gHub->tellReflectionValues(
                    Range::safeFrom(spinRangeMin_->value(), spinRangeMax_->value()),
                    qpair(spinGuessPeakX_->value(), spinGuessPeakY_->value()),
                    fwhm_t(spinGuessFWHM_->value()), invalidateGuesses);
            }
        };

        auto _changeReflData0 = [_newReflData](qreal) { _newReflData(false); };
        auto _changeReflData1 = [_newReflData](qreal) { _newReflData(true); };

        connect(spinRangeMin_, _SLOT_(QDoubleSpinBox, valueChanged, double), _changeReflData1);
        connect(spinRangeMax_, _SLOT_(QDoubleSpinBox, valueChanged, double), _changeReflData1);
        connect(spinGuessPeakX_, _SLOT_(QDoubleSpinBox, valueChanged, double), _changeReflData0);
        connect(spinGuessPeakY_, _SLOT_(QDoubleSpinBox, valueChanged, double), _changeReflData0);
        connect(spinGuessFWHM_, _SLOT_(QDoubleSpinBox, valueChanged, double), _changeReflData0);
    }

    connect(this, &SubframeSetup::currentChanged,
            [this, backgroundTabIndex, reflectionTabIndex](int index) {
        eFittingTab tab;
        if (backgroundTabIndex == index)
            tab = eFittingTab::BACKGROUND;
        else if (reflectionTabIndex == index)
            tab = eFittingTab::REFLECTIONS;
        else
            tab = eFittingTab::NONE;

        gHub->setFittingTab(tab);
    });

    gHub->setFittingTab(eFittingTab::NONE);
}

void SubframeSetup::setToHub() {
    gHub->setGeometry(
        qMax(qreal(Geometry::MIN_DETECTOR_DISTANCE), detDistance_->value()),
        qMax(qreal(Geometry::MIN_DETECTOR_PIXEL_SIZE), detPixelSize_->value()),
        IJ(beamOffsetI_->value(), beamOffsetJ_->value()));
}

void SubframeSetup::setFromHub() {
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

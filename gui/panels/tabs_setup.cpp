// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/tabs_setup.cpp
//! @brief     Implements class TabsSetup
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "tabs_setup.h"
#include "cfg/gui_cfg.h"
#include "fit/fit_fun.h"
#include "models.h"
#include "session.h"
#include "thehub.h"
#include "widgets/tree_views.h" // inheriting from
#include "widgets/widget_makers.h"
#include <QAction>




static qreal safeReal(qreal val) { return qIsFinite(val) ? val : 0.0; }
static str safeRealText(qreal val) { return qIsFinite(val) ? str::number(val) : ""; }


// ************************************************************************** //
//  class ReflectionView (local scope)
// ************************************************************************** //

class ReflectionView : public ListView {
public:
    ReflectionView();

    void addReflection(const QString& peakFunctionName);
    void removeSelected();
    void clear();
    bool hasReflections() const;

    shp_Reflection selectedReflection() const;

    void updateSingleSelection();

private:
    using Model = ReflectionsModel;
    Model* model() const { return static_cast<Model*>(ListView::model()); }

    void selectionChanged(QItemSelection const&, QItemSelection const&);
};

ReflectionView::ReflectionView() : ListView() {
    setModel(gHub->reflectionsModel);
    debug::ensure(dynamic_cast<Model*>(ListView::model()));

    for_i (model()->columnCount())
        resizeColumnToContents(i);
}

void ReflectionView::addReflection(QString const& peakFunctionName) {
    model()->addReflection(peakFunctionName);
    updateSingleSelection();
}

void ReflectionView::removeSelected() {
    int row = currentIndex().row();
    if (row < 0 || model()->rowCount() <= row)
        return;

    model()->remReflection(to_u(row));
    updateSingleSelection();
}

void ReflectionView::clear() {
    for (int row = model()->rowCount(); row-- > 0;) {
        model()->remReflection(to_u(row));
        updateSingleSelection();
    }
}

bool ReflectionView::hasReflections() const {
    return model()->rowCount() > 0;
}

shp_Reflection ReflectionView::selectedReflection() const {
    QList<QModelIndex> indexes = selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
        return shp_Reflection();
    return model()->data(indexes.first(), Model::GetDatasetRole).value<shp_Reflection>();
}

void ReflectionView::updateSingleSelection() {
    ListView::updateSingleSelection();
    gHub->trigger_remReflection->setEnabled(hasReflections());
}

void ReflectionView::selectionChanged(
    QItemSelection const& selected, QItemSelection const& deselected) {
    ListView::selectionChanged(selected, deselected);

    QList<QModelIndex> indexes = selected.indexes();
    gHub->tellSelectedReflection(
        indexes.isEmpty()
            ? shp_Reflection()
            : model()->data(indexes.first(), Model::GetDatasetRole).value<shp_Reflection>());
}


// ************************************************************************** //
//  class TabsSetup
// ************************************************************************** //

QLabel* icon(rcstr fileName) {
    auto l = new QLabel;
    auto h = l->sizeHint().height();
    l->setPixmap(QIcon(fileName).pixmap(QSize(h, h)));
    return l;
}

TabsSetup::TabsSetup() : TabsPanel() {

    int backgroundTabIndex, reflectionTabIndex;

    // ==== image geometry tab ====
    {
        QBoxLayout& box = addTab("Geometry", Qt::Vertical).box();

        connect(gHub, &TheHub::sigGeometryChanged, [this](){ setFromHub(); });

        // widgets

        detDistance_ = spinDoubleCell(gui_cfg::em4_2, Geometry::MIN_DETECTOR_DISTANCE);
        detPixelSize_ = spinDoubleCell(gui_cfg::em4_2, Geometry::MIN_DETECTOR_PIXEL_SIZE);
        detPixelSize_->setDecimals(3);

        detDistance_->setValue(Geometry::DEF_DETECTOR_DISTANCE);
        detPixelSize_->setValue(Geometry::DEF_DETECTOR_PIXEL_SIZE);

        connect(detDistance_, slot(QDoubleSpinBox, valueChanged, double), [this]() { setToHub(); });

        connect(
            detPixelSize_, slot(QDoubleSpinBox, valueChanged, double), [this]() { setToHub(); });

        beamOffsetI_ = spinCell(gui_cfg::em4_2);
        beamOffsetJ_ = spinCell(gui_cfg::em4_2);

        connect(beamOffsetI_, slot(QSpinBox, valueChanged, int), [this]() { setToHub(); });

        connect(beamOffsetJ_, slot(QSpinBox, valueChanged, int), [this]() { setToHub(); });

        cutLeft_ = spinCell(gui_cfg::em4, 0);
        cutTop_ = spinCell(gui_cfg::em4, 0);
        cutRight_ = spinCell(gui_cfg::em4, 0);
        cutBottom_ = spinCell(gui_cfg::em4, 0);

        auto setImageCut = [this](bool isTopOrLeft, int value) {
            debug::ensure(value >= 0);
            if (gHub->toggle_linkCuts->isChecked())
                gHub->setImageCut(
                    isTopOrLeft, true,
                    ImageCut(to_u(value), to_u(value), to_u(value), to_u(value)));
            else
                gHub->setImageCut(
                    isTopOrLeft, false, ImageCut(
                                        to_u(cutLeft_->value()), to_u(cutTop_->value()),
                                        to_u(cutRight_->value()), to_u(cutBottom_->value())));
        };

        connect(cutLeft_, slot(QSpinBox, valueChanged, int), [setImageCut](int value) {
            setImageCut(true, value);
        });

        connect(cutTop_, slot(QSpinBox, valueChanged, int), [setImageCut](int value) {
            setImageCut(true, value);
        });

        connect(cutRight_, slot(QSpinBox, valueChanged, int), [setImageCut](int value) {
            setImageCut(false, value);
        });

        connect(cutBottom_, slot(QSpinBox, valueChanged, int), [setImageCut](int value) {
            setImageCut(false, value);
        });

        // layout

        QGridLayout* grid = gridLayout();
        int row = 0;

        auto add = [&grid, &row](QVector<QWidget*> const& ws, int left = 1) {
            int i = 0, cnt = ws.count();

            QBoxLayout* box = hbox();
            box->addStretch(1);
            while (i < left)
                box->addWidget(ws.at(i++));
            grid->addLayout(box, row, 0);

            box = hbox();
            while (i < cnt)
                box->addWidget(ws.at(i++));
            grid->addLayout(box, row, 1);

            box->addStretch(1);

            row++;
        };

        add({ label("det. distance"), detDistance_, label("mm") });
        add({ label("pixel size"), detPixelSize_, label("mm") });

        add({ label("beam offset X"), beamOffsetI_, label("pix") });
        add({ label("Y"), beamOffsetJ_, label("pix") });

        add({ label("image rotate"), iconButton(gHub->trigger_rotateImage), label("mirror"),
              iconButton(gHub->toggle_mirrorImage) });

        add({ iconButton(gHub->toggle_linkCuts), label("cut"), icon(":/icon/cutLeft"), cutLeft_,
              icon(":/icon/cutRight"), cutRight_ }, 3);
        add({ icon(":/icon/cutTop"), cutTop_, icon(":/icon/cutBottom"), cutBottom_ });

        grid->setColumnStretch(grid->columnCount(), 1);

        box.addLayout(grid);
        box.addStretch();
    }

    // ==== background fit tab ====
    {
        backgroundTabIndex = count();
        QBoxLayout& box = addTab("Background", Qt::Vertical).box();
        QBoxLayout* hb = hbox();
        box.addLayout(hb);

        hb->addWidget(iconButton(gHub->toggle_selRegions));
        hb->addWidget(iconButton(gHub->toggle_showBackground));
        hb->addWidget(iconButton(gHub->trigger_clearBackground));
        hb->addWidget(label("Pol. degree:"));
        hb->addWidget((spinDegree_ = spinCell(gui_cfg::em4, 0, TheHub::MAX_POLYNOM_DEGREE)));
        hb->addStretch();

        box.addStretch(1);

        connect(spinDegree_, slot(QSpinBox, valueChanged, int), [this](int degree) {
                debug::ensure(degree >= 0);
                gHub->setBgPolyDegree(to_u(degree));
            });

        connect(gHub, &TheHub::sigBgChanged, [this](){
                spinDegree_->setValue(to_i(gSession->bgPolyDegree())); });
    }

    // ==== peak fits tab ====
    {
        reflectionTabIndex = count();
        QBoxLayout& box = addTab("Reflections", Qt::Vertical).box();
        QBoxLayout* hb = hbox();
        box.addLayout(hb);

        hb->addWidget(iconButton(gHub->toggle_selRegions));
        hb->addWidget(iconButton(gHub->toggle_showBackground));
        hb->addWidget(iconButton(gHub->trigger_clearReflections));
        hb->addStretch();

        box.addWidget((reflectionView_ = new ReflectionView()));

        hb = hbox();
        box.addLayout(hb);

        comboReflType_ = new QComboBox;
        comboReflType_->addItems(FunctionRegistry::instance()->keys());
        hb->addWidget(comboReflType_);
        hb->addStretch();
        hb->addWidget(iconButton(gHub->trigger_addReflection));
        hb->addWidget(iconButton(gHub->trigger_remReflection));

        QBoxLayout* vb = vbox();
        box.addLayout(vb);

        QGridLayout* gb = gridLayout();
        vb->addLayout(gb);

        gb->addWidget(label("min"), 0, 0);
        gb->addWidget((spinRangeMin_ = spinDoubleCell(gui_cfg::em4_2, .0)), 0, 1);
        spinRangeMin_->setSingleStep(.1);
        gb->addWidget(label("max"), 0, 2);
        gb->addWidget((spinRangeMax_ = spinDoubleCell(gui_cfg::em4_2, .0)), 0, 3);
        spinRangeMax_->setSingleStep(.1);

        gb->addWidget(label("guess x"), 1, 0);
        gb->addWidget((spinGuessPeakX_ = spinDoubleCell(gui_cfg::em4_2, .0)), 1, 1);
        spinGuessPeakX_->setSingleStep(.1);
        gb->addWidget(label("y"), 1, 2);
        gb->addWidget((spinGuessPeakY_ = spinDoubleCell(gui_cfg::em4_2, .0)), 1, 3);
        spinGuessPeakY_->setSingleStep(.1);

        gb->addWidget(label("fwhm"), 2, 0);
        gb->addWidget((spinGuessFWHM_ = spinDoubleCell(gui_cfg::em4_2, .0)), 2, 1);
        spinGuessFWHM_->setSingleStep(.1);

        gb->addWidget(label("fit x"), 3, 0);
        gb->addWidget((readFitPeakX_ = readCell(gui_cfg::em4_2)), 3, 1);
        gb->addWidget(label("y"), 3, 2);
        gb->addWidget((readFitPeakY_ = readCell(gui_cfg::em4_2)), 3, 3);

        gb->addWidget(label("fwhm"), 4, 0);
        gb->addWidget((readFitFWHM_ = readCell(gui_cfg::em4_2)), 4, 1);

        gb->setColumnStretch(4, 1);

        auto updateReflectionControls = [this]() {
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

        updateReflectionControls();

        connect(gHub->trigger_addReflection, &QAction::triggered,
                [this, updateReflectionControls]() {
            reflectionView_->addReflection(comboReflType_->currentText());
            updateReflectionControls();
        });

        connect(gHub->trigger_remReflection, &QAction::triggered,
                [this, updateReflectionControls]() {
            reflectionView_->removeSelected();
            updateReflectionControls();
        });

        connect(gHub->trigger_clearReflections, &QAction::triggered,
                [this, updateReflectionControls]() {
            reflectionView_->clear();
            updateReflectionControls();
        });

        connect(gHub, &TheHub::sigReflectionsChanged,
                [this, updateReflectionControls]() {
                    reflectionView_->updateSingleSelection();
                    updateReflectionControls(); }
            );

        connect(comboReflType_, slot(QComboBox, currentIndexChanged, const QString&),
                [this](const QString& peakFunctionName) {
            gHub->setPeakFunction(peakFunctionName);
        });

        auto setReflControls = [this](shp_Reflection reflection) {
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

                auto& range = reflection->range();
                spinRangeMin_->setValue(safeReal(range.min));
                spinRangeMax_->setValue(safeReal(range.max));

                auto& peakFun = reflection->peakFunction();
                auto& guessedPeak = peakFun.guessedPeak();
                spinGuessPeakX_->setValue(safeReal(guessedPeak.x));
                spinGuessPeakY_->setValue(safeReal(guessedPeak.y));
                spinGuessFWHM_->setValue(safeReal(peakFun.guessedFWHM()));

                auto fittedPeak = peakFun.fittedPeak();
                readFitPeakX_->setText(safeRealText(fittedPeak.x));
                readFitPeakY_->setText(safeRealText(fittedPeak.y));
                readFitFWHM_->setText(safeRealText(peakFun.fittedFWHM()));
            }

            silentSpin_ = false;
        };

        connect(gHub, &TheHub::sigReflectionSelected,
            [setReflControls](shp_Reflection reflection) { setReflControls(reflection); });

        connect(gHub, &TheHub::sigReflectionData,
            [setReflControls](shp_Reflection reflection) { setReflControls(reflection); });

        auto newReflData = [this](bool invalidateGuesses) {
            if (!silentSpin_) {
                gHub->tellReflectionValues(
                    Range::safeFrom(spinRangeMin_->value(), spinRangeMax_->value()),
                    qpair(spinGuessPeakX_->value(), spinGuessPeakY_->value()),
                    fwhm_t(spinGuessFWHM_->value()), invalidateGuesses);
            }
        };

        auto changeReflData0 = [newReflData](qreal) { newReflData(false); };

        auto changeReflData1 = [newReflData](qreal) { newReflData(true); };

        connect(spinRangeMin_, slot(QDoubleSpinBox, valueChanged, double), changeReflData1);
        connect(spinRangeMax_, slot(QDoubleSpinBox, valueChanged, double), changeReflData1);
        connect(spinGuessPeakX_, slot(QDoubleSpinBox, valueChanged, double), changeReflData0);
        connect(spinGuessPeakY_, slot(QDoubleSpinBox, valueChanged, double), changeReflData0);
        connect(spinGuessFWHM_, slot(QDoubleSpinBox, valueChanged, double), changeReflData0);
    }

    connect(this, &TabsSetup::currentChanged,
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

void TabsSetup::setToHub() {
    gHub->setGeometry(
        preal(qMax(qreal(Geometry::MIN_DETECTOR_DISTANCE), detDistance_->value())),
        preal(qMax(qreal(Geometry::MIN_DETECTOR_PIXEL_SIZE), detPixelSize_->value())),
        IJ(beamOffsetI_->value(), beamOffsetJ_->value()));
}

void TabsSetup::setFromHub() {
    auto& g = gSession->geometry();

    detDistance_->setValue(g.detectorDistance);
    detPixelSize_->setValue(g.pixSize);

    beamOffsetI_->setValue(g.midPixOffset.i);
    beamOffsetJ_->setValue(g.midPixOffset.j);

    auto cut = gSession->imageCut();

    cutLeft_->setValue(to_i(cut.left));
    cutTop_->setValue(to_i(cut.top));
    cutRight_->setValue(to_i(cut.right));
    cutBottom_->setValue(to_i(cut.bottom));
}




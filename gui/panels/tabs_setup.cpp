/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "tabs_setup.h"
#include "gui_cfg.h"
#include "thehub.h"
#include "typ/typ_geometry.h"
#include "views.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ReflectionView : public views::ListView {
  CLASS(ReflectionView) SUPER(views::ListView)
public:
  ReflectionView(TheHub&);

  void addReflection(uint type);
  void removeSelected();
  void clear();
  bool hasReflections() const;

  calc::shp_Reflection selectedReflection() const;

  void updateSingleSelection();

protected:
  using Model = models::ReflectionsModel;
  Model *model() const { return static_cast<Model*>(super::model()); }

  void selectionChanged(QItemSelection const&, QItemSelection const&);
};

ReflectionView::ReflectionView(TheHub &hub) : super(hub) {
  setModel(&hub.reflectionsModel);
  EXPECT(dynamic_cast<Model*>(super::model()))

  for_i (model()->columnCount())
    resizeColumnToContents(i);
}

void ReflectionView::addReflection(uint type) {
  type = qBound(0u, type, uint(fit::ePeakType::NUM_TYPES));
  model()->addReflection(fit::ePeakType(type));
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
  for (int row = model()->rowCount(); row-- > 0; ) {
    model()->remReflection(to_u(row));
    updateSingleSelection();
  }
}

bool ReflectionView::hasReflections() const {
  return model()->rowCount() > 0;
}

calc::shp_Reflection ReflectionView::selectedReflection() const {
  auto indexes = selectionModel()->selectedIndexes();
  if (indexes.isEmpty())
    return calc::shp_Reflection();
  return model()
      ->data(indexes.first(), Model::GetDatasetRole)
      .value<calc::shp_Reflection>();
}

void ReflectionView::updateSingleSelection() {
  super::updateSingleSelection();
  hub_.actions.remReflection->setEnabled(hasReflections());
}

void ReflectionView::selectionChanged(QItemSelection const& selected,
                                      QItemSelection const& deselected) {
  super::selectionChanged(selected, deselected);

  auto indexes = selected.indexes();
  tellSelectedReflection(indexes.isEmpty()
      ? calc::shp_Reflection()
      : model()->data(indexes.first(), Model::GetDatasetRole).value<calc::shp_Reflection>());
}

//------------------------------------------------------------------------------

static qreal safeReal(qreal val) {
  return qIsFinite(val) ? val : 0.0;
}

static str safeRealText(qreal val) {
  return qIsFinite(val) ? str::number(val) : EMPTY_STR;
}

TabsSetup::TabsSetup(TheHub& hub) : super(hub) {
  auto& actions = hub_.actions;

  int backgroundTabIndex, reflectionTabIndex;

  {
    auto& box = addTab("Geometry", Qt::Vertical).box();

    onSigGeometryChanged([this]() {
      setFromHub();
    });

    // widgets

    detDistance_  = spinDoubleCell(gui_cfg::em4_2, typ::Geometry::MIN_DETECTOR_DISTANCE);
    detPixelSize_ = spinDoubleCell(gui_cfg::em4_2, typ::Geometry::MIN_DETECTOR_PIXEL_SIZE);
    detPixelSize_->setDecimals(3);

    detDistance_->setValue(typ::Geometry::DEF_DETECTOR_DISTANCE);
    detPixelSize_->setValue(typ::Geometry::DEF_DETECTOR_PIXEL_SIZE);

    connect(detDistance_, slot(QDoubleSpinBox,valueChanged,double), [this]() {
      setToHub();
    });

    connect(detPixelSize_, slot(QDoubleSpinBox,valueChanged,double), [this]() {
      setToHub();
    });

    beamOffsetI_  = spinCell(gui_cfg::em4_2);
    beamOffsetJ_  = spinCell(gui_cfg::em4_2);

    connect(beamOffsetI_, slot(QSpinBox,valueChanged,int), [this]() {
      setToHub();
    });

    connect(beamOffsetJ_, slot(QSpinBox,valueChanged,int), [this]() {
      setToHub();
    });

    cutLeft_   = spinCell(gui_cfg::em4, 0);
    cutTop_    = spinCell(gui_cfg::em4, 0);
    cutRight_  = spinCell(gui_cfg::em4, 0);
    cutBottom_ = spinCell(gui_cfg::em4, 0);

    auto setImageCut = [this](bool topLeft, int value) {
      EXPECT(value >= 0)
      if (hub_.actions.linkCuts->isChecked())
        hub_.setImageCut(topLeft, true, typ::ImageCut(
                         to_u(value), to_u(value), to_u(value), to_u(value)));
      else
        hub_.setImageCut(topLeft, false,
                         typ::ImageCut(to_u(cutLeft_->value()),  to_u(cutTop_->value()),
                                       to_u(cutRight_->value()), to_u(cutBottom_->value())));
    };

    connect(cutLeft_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
      setImageCut(true, value);
    });

    connect(cutTop_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
      setImageCut(true, value);
    });

    connect(cutRight_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
      setImageCut(false, value);
    });

    connect(cutBottom_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
      setImageCut(false, value);
    });

    // layout

    auto grid = gridLayout(); int row = 0;

    auto add = [&grid, &row](QVector<QWidget*> const& ws, int left = 1) {
      int i = 0, cnt = ws.count();

      auto box = hbox(); box->addStretch(1);
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

    add({ label("det. distance"), detDistance_,  label("mm") });
    add({ label("pixel size"),    detPixelSize_, label("mm") });

    add({ label("beam offset X"), beamOffsetI_,  label("pix") });
    add({ label("Y"),             beamOffsetJ_,  label("pix") });

    add({ label("image rotate"),  iconButton(hub_.actions.rotateImage),
                                  label("mirror"), iconButton(hub_.actions.mirrorImage) });

    add({ iconButton(hub_.actions.linkCuts), label("cut"), icon(":/icon/cutLeft"),
                                  cutLeft_, icon(":/icon/cutRight"), cutRight_ }, 3);
    add({ icon(":/icon/cutTop"),  cutTop_,
                                  icon(":/icon/cutBottom"), cutBottom_});

    grid->addColumnStretch(1);

    box.addLayout(grid);
    box.addStretch();
  }\

  {
    backgroundTabIndex = count();
    auto& box = addTab("Background", Qt::Vertical).box();
    auto hb = hbox();
    box.addLayout(hb);

    hb->addWidget(iconButton(actions.selRegions));
    hb->addWidget(iconButton(actions.showBackground));
    hb->addWidget(iconButton(actions.clearBackground));
    hb->addWidget(label("Pol. degree:"));
    hb->addWidget((spinDegree_ = spinCell(gui_cfg::em4, 0, TheHub::MAX_POLYNOM_DEGREE)));
    hb->addStretch();

    box.addStretch(1);

    connect(spinDegree_, slot(QSpinBox,valueChanged,int), [this](int degree) {
      EXPECT(degree >= 0)
      hub_.setBgPolyDegree(to_u(degree));
    });

    onSigBgChanged([this]() {
      spinDegree_->setValue(to_i(hub_.bgPolyDegree()));
    });
  }

  {
    reflectionTabIndex = count();
    auto& box = addTab("Reflections", Qt::Vertical).box();
    auto hb = hbox();
    box.addLayout(hb);

    hb->addWidget(iconButton(actions.selRegions));
    hb->addWidget(iconButton(actions.showBackground));
    hb->addWidget(iconButton(actions.clearReflections));
    hb->addStretch();

    box.addWidget((reflectionView_ = new ReflectionView(hub_)));

    hb = hbox();
    box.addLayout(hb);

    hb->addWidget((comboReflType_ = comboBox(calc::Reflection::typeStrLst())));
    hb->addStretch();
    hb->addWidget(iconButton(actions.addReflection));
    hb->addWidget(iconButton(actions.remReflection));

    auto vb = vbox();
    box.addLayout(vb);

    auto gb = gridLayout();
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

    connect(actions.addReflection, &QAction::triggered, [this,updateReflectionControls]() {
      int i = comboReflType_->currentIndex();
      EXPECT(i >= 0)
      reflectionView_->addReflection(to_u(i));
      updateReflectionControls();
    });

    connect(actions.remReflection, &QAction::triggered, [this,updateReflectionControls]() {
      reflectionView_->removeSelected();
      updateReflectionControls();
    });

    connect(actions.clearReflections, &QAction::triggered, [this,updateReflectionControls]() {
      reflectionView_->clear();
      updateReflectionControls();
    });

    onSigReflectionsChanged([this,updateReflectionControls]() {
      reflectionView_->updateSingleSelection();
      updateReflectionControls();
    });

    connect(comboReflType_, slot(QComboBox,currentIndexChanged,int), [this](int index) {
      hub_.setReflType(fit::ePeakType(index));
    });

    auto setReflControls = [this](calc::shp_Reflection reflection) {
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
          comboReflType_->setCurrentIndex(int(reflection->type()));
        }

        auto& range = reflection->range();
        spinRangeMin_->setValue(safeReal(range.min));
        spinRangeMax_->setValue(safeReal(range.max));

        auto& peakFun     = reflection->peakFunction();
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

    onSigReflectionSelected([setReflControls](calc::shp_Reflection reflection) {
      setReflControls(reflection);
    });

    onSigReflectionData([setReflControls](calc::shp_Reflection reflection) {
      setReflControls(reflection);
    });

    auto newReflData = [this](bool invalidateGuesses) {
      if (!silentSpin_) {
        tellReflectionValues(
            typ::Range::safeFrom(spinRangeMin_->value(), spinRangeMax_->value()),
            typ::XY(spinGuessPeakX_->value(), spinGuessPeakY_->value()),
            fwhm_t(spinGuessFWHM_->value()), invalidateGuesses);
      }
    };

    auto changeReflData0 = [newReflData](qreal) { newReflData(false); };

    auto changeReflData1 = [newReflData](qreal) { newReflData(true); };

    connect(spinRangeMin_,   slot(QDoubleSpinBox,valueChanged,double), changeReflData1);
    connect(spinRangeMax_,   slot(QDoubleSpinBox,valueChanged,double), changeReflData1);
    connect(spinGuessPeakX_, slot(QDoubleSpinBox,valueChanged,double), changeReflData0);
    connect(spinGuessPeakY_, slot(QDoubleSpinBox,valueChanged,double), changeReflData0);
    connect(spinGuessFWHM_,  slot(QDoubleSpinBox,valueChanged,double), changeReflData0);
  }

  connect(this, &Cls::currentChanged, [this, backgroundTabIndex, reflectionTabIndex](int index) {
    eFittingTab tab;
    if (backgroundTabIndex == index)
      tab = eFittingTab::BACKGROUND;
    else if (reflectionTabIndex == index)
      tab = eFittingTab::REFLECTIONS;
    else
      tab = eFittingTab::NONE;

    hub_.setFittingTab(tab);
  });

  hub_.setFittingTab(eFittingTab::NONE);
}

void TabsSetup::setToHub() {
  hub_.setGeometry(
    preal(qMax(qreal(typ::Geometry::MIN_DETECTOR_DISTANCE),   detDistance_->value())),
    preal(qMax(qreal(typ::Geometry::MIN_DETECTOR_PIXEL_SIZE), detPixelSize_->value())),
    typ::IJ(beamOffsetI_->value(), beamOffsetJ_->value())
  );
}

void TabsSetup::setFromHub() {
  auto& g = hub_.geometry();

  detDistance_->setValue(g.detectorDistance);
  detPixelSize_->setValue(g.pixSize);

  beamOffsetI_->setValue(g.midPixOffset.i);
  beamOffsetJ_->setValue(g.midPixOffset.j);

  auto cut = hub_.imageCut();

  cutLeft_  ->setValue(to_i(cut.left));
  cutTop_   ->setValue(to_i(cut.top));
  cutRight_ ->setValue(to_i(cut.right));
  cutBottom_->setValue(to_i(cut.bottom));
}

//------------------------------------------------------------------------------
}}
// eof

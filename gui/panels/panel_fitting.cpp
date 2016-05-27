// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_fitting.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "panel_fitting.h"
#include "core_fit_limits.h"
#include "core_reflection.h"
#include "thehub.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ReflectionView : public views::ListView {
  SUPER(ReflectionView, views::ListView)
public:
  ReflectionView(TheHub&);

  void addReflection(int type);
  void removeSelected();
  bool hasReflections() const;

  core::shp_Reflection selectedReflection() const;

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

void ReflectionView::addReflection(int type) {
  using eType = core::ePeakType;
  model()->addReflection(
      (eType)qBound(0, type, (int)eType::NUM_TYPES));  // make safe
  updateSingleSelection();
}

void ReflectionView::removeSelected() {
  int row = currentIndex().row();
  if (row < 0 || model()->rowCount() <= row) return;

  model()->remReflection(row);
  updateSingleSelection();
}

bool ReflectionView::hasReflections() const {
  return model()->rowCount() > 0;
}

core::shp_Reflection ReflectionView::selectedReflection() const {
  auto indexes = selectionModel()->selectedIndexes();
  if (indexes.isEmpty()) return core::shp_Reflection();
  return model()
      ->data(indexes.first(), Model::GetDatasetRole)
      .value<core::shp_Reflection>();
}

void ReflectionView::updateSingleSelection() {
  super::updateSingleSelection();
  hub_.actions.remReflection->setEnabled(hasReflections());
}

void ReflectionView::selectionChanged(QItemSelection const &selected,
                                      QItemSelection const &deselected) {
  super::selectionChanged(selected, deselected);

  auto indexes = selected.indexes();
  tellSelectedReflection(
      indexes.isEmpty() ? core::shp_Reflection()
                        : model()
                              ->data(indexes.first(), Model::GetDatasetRole)
                              .value<core::shp_Reflection>());
}

//------------------------------------------------------------------------------

Fitting::Fitting(TheHub &hub) : super(hub), silentSpin_(false) {

  auto &actions = hub_.actions;
  auto  tools   = [actions]() {
    auto hb = hbox();
    hb->addWidget(iconButton(actions.fitRegions));
    hb->addWidget(iconButton(actions.fitBgClear));
    hb->addWidget(iconButton(actions.fitBgShow));
    hb->addStretch();
    return hb;
  };

  {  // background
    auto &tab = addTab("Background");
    tab.box().addLayout(tools());

    auto hb = hbox();
    tab.box().addLayout(hb);
    hb->addWidget(label("Polynom degree:"));
    hb->addWidget(
        (spinDegree_ = spinCell(4, 0, core::fit::MAX_POLYNOM_DEGREE)));
    hb->addStretch();

    tab.box().addStretch();

    connect(spinDegree_,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [this](int degree) { hub_.setBgPolyDegree(degree); });
  }

  {  // reflections
    auto &tab = addTab("Reflections");
    tab.box().addLayout(tools());

    tab.box().addWidget((reflectionView_ = new ReflectionView(hub_)));

    auto hb = hbox();
    tab.box().addLayout(hb);

    hb->addWidget((comboReflType_ = comboBox(core::Reflection::typeStrLst())));
    hb->addStretch();
    hb->addWidget(iconButton(actions.addReflection));
    hb->addWidget(iconButton(actions.remReflection));

    auto vb = vbox();
    tab.box().addLayout(vb);

    auto gb = gridLayout();
    vb->addLayout(gb);

    gb->addWidget(label("min"), 0, 0);
    gb->addWidget((spinRangeMin_ = spinCell(6, .0)), 0, 1);
    spinRangeMin_->setSingleStep(.1);
    gb->addWidget(label("max"), 0, 2);
    gb->addWidget((spinRangeMax_ = spinCell(6, .0)), 0, 3);
    spinRangeMax_->setSingleStep(.1);

    gb->addWidget(label("guess x"), 1, 0);
    gb->addWidget((spinGuessPeakX_ = spinCell(6, .0)), 1, 1);
    spinGuessPeakX_->setSingleStep(.1);
    gb->addWidget(label("y"), 1, 2);
    gb->addWidget((spinGuessPeakY_ = spinCell(6, .0)), 1, 3);
    spinGuessPeakY_->setSingleStep(.1);

    gb->addWidget(label("fwhm"), 2, 0);
    gb->addWidget((spinGuessFWHM_ = spinCell(6, .0)), 2, 1);
    spinGuessFWHM_->setSingleStep(.1);

    gb->addWidget(label("fit x"), 3, 0);
    gb->addWidget((readFitPeakX_ = readCell(6)), 3, 1);
    gb->addWidget(label("y"), 3, 2);
    gb->addWidget((readFitPeakY_ = readCell(6)), 3, 3);

    gb->addWidget(label("fwhm"), 4, 0);
    gb->addWidget((readFitFWHM_ = readCell(6)), 4, 1);

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
      reflectionView_->addReflection(comboReflType_->currentIndex());
      updateReflectionControls();
    });

    connect(actions.remReflection, &QAction::triggered, [this,updateReflectionControls]() {
      reflectionView_->removeSelected();
      updateReflectionControls();
    });

    onSigReflectionsChanged([this,updateReflectionControls]() {
      reflectionView_->updateSingleSelection();
      updateReflectionControls();
    });

    connect(comboReflType_, static_cast<void (QComboBox::*)(int)>(
                                &QComboBox::currentIndexChanged),
            [this](int index) { hub_.setReflType((core::ePeakType)index); });

    onSigReflectionSelected([this](core::shp_Reflection reflection) {
      setReflControls(reflection);
    });

    onSigReflectionData([this](core::shp_Reflection reflection) {
      setReflControls(reflection);
    });

    auto newReflData = [this](bool invalidateGuesses) {
      if (!silentSpin_) {
        tellReflectionValues(
            core::Range::safeFrom(spinRangeMin_->value(),
                                  spinRangeMax_->value()),
            core::XY(spinGuessPeakX_->value(), spinGuessPeakY_->value()),
            spinGuessFWHM_->value(), invalidateGuesses);
      }
    };

    auto changeReflData0 = [newReflData](qreal) { newReflData(false); };

    auto changeReflData1 = [newReflData](qreal) { newReflData(true); };

    connect(spinRangeMin_, static_cast<void (QDoubleSpinBox::*)(qreal)>(
                               &QDoubleSpinBox::valueChanged),
            changeReflData1);
    connect(spinRangeMax_, static_cast<void (QDoubleSpinBox::*)(qreal)>(
                               &QDoubleSpinBox::valueChanged),
            changeReflData1);
    connect(spinGuessPeakX_, static_cast<void (QDoubleSpinBox::*)(qreal)>(
                                 &QDoubleSpinBox::valueChanged),
            changeReflData0);
    connect(spinGuessPeakY_, static_cast<void (QDoubleSpinBox::*)(qreal)>(
                                 &QDoubleSpinBox::valueChanged),
            changeReflData0);
    connect(spinGuessFWHM_, static_cast<void (QDoubleSpinBox::*)(qreal)>(
                                &QDoubleSpinBox::valueChanged),
            changeReflData0);
  }

  connect(this, &thisClass::currentChanged, [this](int index) {
    hub_.setFittingTab((eFittingTab)index);
  });

  hub_.setFittingTab(eFittingTab::BACKGROUND);
}

static qreal safeReal(qreal val) {
  return qIsFinite(val) ? val : 0.0;
}

static str safeRealText(qreal val) {
  return qIsFinite(val) ? str::number(val) : EMPTY_STR;
}

void Fitting::setReflControls(core::shp_Reflection reflection) {
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
      comboReflType_->setCurrentIndex((int)reflection->type());
    }

    auto &range = reflection->range();
    spinRangeMin_->setValue(safeReal(range.min));
    spinRangeMax_->setValue(safeReal(range.max));

    auto &peakFun     = reflection->peakFunction();
    auto &guessedPeak = peakFun.guessedPeak();
    spinGuessPeakX_->setValue(safeReal(guessedPeak.x));
    spinGuessPeakY_->setValue(safeReal(guessedPeak.y));
    spinGuessFWHM_->setValue(safeReal(peakFun.guessedFWHM()));

    auto fittedPeak = peakFun.fittedPeak();
    readFitPeakX_->setText(safeRealText(fittedPeak.x));
    readFitPeakY_->setText(safeRealText(fittedPeak.y));
    readFitFWHM_->setText(safeRealText(peakFun.fittedFWHM()));
  }

  silentSpin_ = false;
}

//------------------------------------------------------------------------------
}}
// eof

// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_fitting.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "panel_fitting.h"
#include "calc/calc_reflection.h"
#include "thehub.h"
#include "views.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ReflectionView : public views::ListView {
  CLS(ReflectionView) SUPER(views::ListView)
public:
  ReflectionView(TheHub&);

  void addReflection(uint type);
  void removeSelected();
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

Fitting::Fitting(TheHub &hub)
: super(hub), silentSpin_(false)
{
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
    hb->addWidget(label("Pol. degree:"));
    hb->addWidget((spinDegree_ = spinCell(4, 0, TheHub::MAX_POLYNOM_DEGREE)));
    hb->addStretch();

    tab.box().addStretch();

    connect(spinDegree_, slot(QSpinBox,valueChanged,int), [this](int degree) {
      EXPECT(degree >= 0)
      hub_.setBgPolyDegree(to_u(degree));
    });

    onSigBgChanged([this]() {
      spinDegree_->setValue(to_i(hub_.bgPolyDegree()));
    });
  }

  {  // reflections
    auto &tab = addTab("Reflections");
    tab.box().addLayout(tools());

    tab.box().addWidget((reflectionView_ = new ReflectionView(hub_)));

    auto hb = hbox();
    tab.box().addLayout(hb);

    hb->addWidget((comboReflType_ = comboBox(calc::Reflection::typeStrLst())));
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
      int i = comboReflType_->currentIndex();
      EXPECT(i >= 0)
      reflectionView_->addReflection(to_u(i));
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

    connect(comboReflType_, slot(QComboBox,currentIndexChanged,int), [this](int index) {
      hub_.setReflType(fit::ePeakType(index));
    });

    onSigReflectionSelected([this](calc::shp_Reflection reflection) {
      setReflControls(reflection);
    });

    onSigReflectionData([this](calc::shp_Reflection reflection) {
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

  connect(this, &Cls::currentChanged, [this](int index) {
    hub_.setFittingTab(eFittingTab(index));
  });

  hub_.setFittingTab(eFittingTab::BACKGROUND);
}

static qreal safeReal(qreal val) {
  return qIsFinite(val) ? val : 0.0;
}

static str safeRealText(qreal val) {
  return qIsFinite(val) ? str::number(val) : EMPTY_STR;
}

void Fitting::setReflControls(calc::shp_Reflection reflection) {
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

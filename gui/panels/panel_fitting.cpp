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
#include "thehub.h"
#include "core_fit_limits.h"
#include <QAction>
#include <QApplication>

namespace gui { namespace panel {
//------------------------------------------------------------------------------

ReflectionView::ReflectionView(TheHub& theHub)
: super(theHub), _model(theHub.reflectionViewModel) {
  setModel(&_model);
  for_i (_model.columnCount())
    resizeColumnToContents(i);
}

void ReflectionView::addReflection(int type) {
  using eType = core::ePeakType;
  _model.addReflection((eType)qBound(0,type,(int)eType::NUM_TYPES)); // make safe
  update();
}

void ReflectionView::removeSelected() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  int row = index.row();
  index = (row < _model.rowCount()) ? index : index.sibling(row-1,0);
  _model.remReflection(row);
  update();
}

bool ReflectionView::hasReflections() const {
  return _model.rowCount() > 0;
}

void ReflectionView::update() {
  auto index = currentIndex();
  _model.signalReset();
  // keep the current index, or select the last item
  setCurrentIndex(index.isValid()
    ? index
    : _model.index(qMax(0,_model.rowCount()-1),0));

  _theHub.actions.remReflection->setEnabled(hasReflections());
}

void ReflectionView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  _theHub.setSelectedReflection(indexes.isEmpty()
    ? core::shp_Reflection()
    : _model.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Reflection>());
}

//------------------------------------------------------------------------------

Fitting::Fitting(TheHub& theHub_)
: super(theHub_), _silentSpin(false) {

  auto &actions = _theHub.actions;
  auto tools = [actions]() {
    auto hb = hbox();
    hb->addWidget(iconButton(actions.fitTool));
    hb->addWidget(iconButton(actions.fitBgClear));
    hb->addWidget(iconButton(actions.fitShow));
    hb->addStretch();
    return hb;
  };

  { // background
    auto &tab = addTab("Background",Qt::Vertical);
    tab.box->addLayout(tools());

    auto hb = hbox();
    tab.box->addLayout(hb);
    hb->addWidget(label("Polynomial degree:"));
    hb->addWidget((_spinDegree = spinCell(4,0,core::fit::MAX_POLYNOMIAL_DEGREE)));
    hb->addStretch();

    tab.box->addStretch();

    connect(_spinDegree, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int degree) {
      _theHub.setBackgroundPolynomialDegree(degree);
    });
  }

  { // reflections
    auto &tab = addTab("Reflections",Qt::Vertical);
    tab.box->addLayout(tools());

    tab.box->addWidget((_reflectionView = new ReflectionView(_theHub)));

    auto hb = hbox();
    tab.box->addLayout(hb);

    hb->addWidget((_comboReflType = comboBox(core::Reflection::typeStrLst())));
    hb->addStretch();
    hb->addWidget(iconButton(actions.addReflection));
    hb->addWidget(iconButton(actions.remReflection));

    auto vb = vbox();
    tab.box->addLayout(vb);

    auto gb = gridLayout();
    vb->addLayout(gb);

    gb->addWidget(label("min"),                       0, 0);
    gb->addWidget((_spinRangeMin   = spinCell(6,.0)),  0, 1);
    _spinRangeMin->setSingleStep(.1);
    gb->addWidget(label("max"),                       0, 2);
    gb->addWidget((_spinRangeMax   = spinCell(6,.0)),  0, 3);
    _spinRangeMax->setSingleStep(.1);

    gb->addWidget(label("guess x"),                    1, 0);
    gb->addWidget((_spinGuessPeakX = spinCell(6,.0)),  1, 1);
    _spinGuessPeakX->setSingleStep(.1);
    gb->addWidget(label("y"),                         1, 2);
    gb->addWidget((_spinGuessPeakY = spinCell(6,.0)),  1, 3);
    _spinGuessPeakY->setSingleStep(.1);

    gb->addWidget(label("fwhm"),                      2, 0);
    gb->addWidget((_spinGuessFWHM  = spinCell(6,.0)),  2, 1);
    _spinGuessFWHM->setSingleStep(.1);

    gb->addWidget(label("fit x"),                     3, 0);
    gb->addWidget((_readFitPeakX   = readCell(6)),     3, 1);
    gb->addWidget(label("y"),                         3, 2);
    gb->addWidget((_readFitPeakY   = readCell(6)),     3, 3);

    gb->addWidget(label("fwhm"),                      4, 0);
    gb->addWidget((_readFitFWHM    = readCell(6)),     4, 1);

    gb->setColumnStretch(4,1);

    updateReflectionControls();

    connect(actions.addReflection, &QAction::triggered, [this]() {
      _reflectionView->addReflection(_comboReflType->currentIndex());
      updateReflectionControls();
    });

    connect(actions.remReflection, &QAction::triggered, [this]() {
      _reflectionView->removeSelected();
      updateReflectionControls();
    });

    connect(&_theHub, &TheHub::reflectionsChanged, [this]() {
      _reflectionView->update();
      updateReflectionControls();
    });

    connect(_comboReflType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
      _theHub.setReflType((core::ePeakType)index);
    });

    connect(&_theHub, &TheHub::reflectionSelected, [this](core::shp_Reflection reflection) {
      setReflControls(reflection);
    });

    connect(&_theHub, &TheHub::reflectionData, [this](core::shp_Reflection reflection) {
      setReflControls(reflection);
    });

    auto newReflData = [this](bool invalidateGuesses) {
      if (!_silentSpin) {
        _theHub.newReflectionData(
          core::Range::safeFrom(_spinRangeMin->value(),_spinRangeMax->value()),
          core::XY(_spinGuessPeakX->value(),_spinGuessPeakY->value()),
          _spinGuessFWHM->value(), invalidateGuesses);
      }
    };

    auto changeReflData0 = [newReflData](qreal) {
      newReflData(false);
    };

    auto changeReflData1 = [newReflData](qreal) {
      newReflData(true);
    };

    connect(_spinRangeMin,   static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData1);
    connect(_spinRangeMax,   static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData1);
    connect(_spinGuessPeakX, static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData0);
    connect(_spinGuessPeakY, static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData0);
    connect(_spinGuessFWHM,  static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData0);
  }

  connect(this, &thisClass::currentChanged, [this](int index) {
    _theHub.setFittingTab(index);
  });

  _theHub.setFittingTab(0);
}

void Fitting::enableReflControls(bool on) {
  _spinRangeMin->setEnabled(on);
  _spinRangeMax->setEnabled(on);
  _spinGuessPeakX->setEnabled(on);
  _spinGuessPeakY->setEnabled(on);
  _spinGuessFWHM->setEnabled(on);
  _readFitPeakX->setEnabled(on);
  _readFitPeakY->setEnabled(on);
  _readFitFWHM->setEnabled(on);
}

// TODO move to core_types ?

static qreal safeReal(qreal val) {
  return qIsFinite(val) ? val : 0.0;
}

static str safeRealText(qreal val) {
  return qIsFinite(val) ? str("%1").arg(val) : EMPTY_STR;
}

void Fitting::setReflControls(core::shp_Reflection const& reflection) {
  _silentSpin = true;

  if (reflection.isNull()) {
    // do not set comboReflType - we want it to stay as it is
    _spinRangeMin->setValue(0);
    _spinRangeMax->setValue(0);
    _spinGuessPeakX->setValue(0);
    _spinGuessPeakY->setValue(0);
    _spinGuessFWHM->setValue(0);
    _readFitPeakX->clear();
    _readFitPeakY->clear();
    _readFitFWHM->clear();
  } else {
    _comboReflType->setCurrentIndex((int)reflection->type());

    auto &range = reflection->range();
    _spinRangeMin->setValue(safeReal(range.min));
    _spinRangeMax->setValue(safeReal(range.max));

    auto &peakFun = reflection->peakFunction();
    auto &guessedPeak = peakFun.guessedPeak();
    _spinGuessPeakX->setValue(safeReal(guessedPeak.x));
    _spinGuessPeakY->setValue(safeReal(guessedPeak.y));
    _spinGuessFWHM->setValue(safeReal(peakFun.guessedFWHM()));

    auto fittedPeak = peakFun.fittedPeak();
    _readFitPeakX->setText(safeRealText(fittedPeak.x));
    _readFitPeakY->setText(safeRealText(fittedPeak.y));
    _readFitFWHM->setText(safeRealText(peakFun.fittedFWHM()));
  }

  _silentSpin = false;
}

void Fitting::updateReflectionControls() {
  _reflectionView->hasReflections() ? enableReflControls(true) : enableReflControls(false);
}

//------------------------------------------------------------------------------
  }}
// eof

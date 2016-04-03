#include "panel_fitting.h"
#include "thehub.h"
#include "core_fit_limits.h"
#include <QAction>
#include <QApplication>

namespace panel {
//------------------------------------------------------------------------------

ReflectionView::ReflectionView(TheHub& theHub)
: super(theHub), model(theHub.reflectionViewModel) {
  setModel(&model);
  for_i (model.columnCount())
    resizeColumnToContents(i);
}

void ReflectionView::addReflection(int type) {
  using eType = core::ePeakType;
  model.addReflection((eType)qBound(0,type,(int)eType::NUM_TYPES)); // make safe
  update();
}

void ReflectionView::removeSelected() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  int row = index.row();
  index = (row < model.rowCount()) ? index : index.sibling(row-1,0);
  model.remReflection(row);
  update();
}

bool ReflectionView::hasReflections() const {
  return model.rowCount() > 0;
}

void ReflectionView::update() {
  auto index = currentIndex();
  model.signalReset();
  // keep the current index, or select the last item
  setCurrentIndex(index.isValid()
    ? index
    : model.index(qMax(0,model.rowCount()-1),0));

  theHub.actReflectionRemove->setEnabled(hasReflections());
}

void ReflectionView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  theHub.setSelectedReflection(indexes.isEmpty()
    ? core::shp_Reflection()
    : model.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Reflection>());
}

//------------------------------------------------------------------------------

Fitting::Fitting(TheHub& theHub_)
: super(theHub_), silentSpin(false) {

//  box->addLayout(hb);

  auto tools = [this]() {
    auto hb = hbox();
    hb->addWidget(iconButton(theHub.actFitTool));
    hb->addWidget(iconButton(theHub.actFitBgClear));
    hb->addWidget(iconButton(theHub.actFitShow));
    hb->addStretch();
    return hb;
  };

  { // background
    auto &tab = addTab("Background",Qt::Vertical);
    tab.box->addLayout(tools());

    auto hb = hbox();
    tab.box->addLayout(hb);
    hb->addWidget(label("Polynomial degree:"));
    hb->addWidget((spinDegree = spinCell(4,0,core::fit::MAX_BACKGROUND_POLYNOMIAL_DEGREE)));
    hb->addStretch();

    tab.box->addStretch();

    connect(spinDegree, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int degree) {
      theHub.setBackgroundPolynomialDegree(degree);
    });
  }

  { // reflections
    auto &tab = addTab("Reflections",Qt::Vertical);
    tab.box->addLayout(tools());

    tab.box->addWidget((reflectionView = new ReflectionView(theHub)));

    auto hb = hbox();
    tab.box->addLayout(hb);

    hb->addWidget((comboReflType = comboBox(core::Reflection::reflTypes())));
    hb->addStretch();
    hb->addWidget(iconButton(theHub.actReflectionAdd));
    hb->addWidget(iconButton(theHub.actReflectionRemove));

    auto vb = vbox();
    tab.box->addLayout(vb);

    auto gb = gridLayout();
    vb->addLayout(gb);

    gb->addWidget(label("min"),                       0, 0);
    gb->addWidget((spinRangeMin   = spinCell(6,.0)),  0, 1);
    spinRangeMin->setSingleStep(.1);
    gb->addWidget(label("max"),                       0, 2);
    gb->addWidget((spinRangeMax   = spinCell(6,.0)),  0, 3);
    spinRangeMax->setSingleStep(.1);

    gb->addWidget(label("guess x"),                    1, 0);
    gb->addWidget((spinGuessPeakX = spinCell(6,.0)),  1, 1);
    spinGuessPeakX->setSingleStep(.1);
    gb->addWidget(label("y"),                         1, 2);
    gb->addWidget((spinGuessPeakY = spinCell(6,.0)),  1, 3);
    spinGuessPeakY->setSingleStep(.1);

    gb->addWidget(label("fwhm"),                      2, 0);
    gb->addWidget((spinGuessFwhm  = spinCell(6,.0)),  2, 1);
    spinGuessFwhm->setSingleStep(.1);

    gb->addWidget(label("fit x"),                     3, 0);
    gb->addWidget((readFitPeakX   = readCell(6)),     3, 1);
    gb->addWidget(label("y"),                         3, 2);
    gb->addWidget((readFitPeakY   = readCell(6)),     3, 3);

    gb->addWidget(label("fwhm"),                      4, 0);
    gb->addWidget((readFitFwhm    = readCell(6)),     4, 1);

    gb->setColumnStretch(4,1);

    updateReflectionControls();

    connect(theHub.actReflectionAdd, &QAction::triggered, [this]() {
      reflectionView->addReflection(comboReflType->currentIndex());
      updateReflectionControls();
    });

    connect(theHub.actReflectionRemove, &QAction::triggered, [this]() {
      reflectionView->removeSelected();
      updateReflectionControls();
    });

    connect(&theHub, &TheHub::reflectionsChanged, [this]() {
      reflectionView->update();
      updateReflectionControls();
    });

    connect(comboReflType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
      theHub.setReflType((core::ePeakType)index);
    });

    connect(&theHub, &TheHub::reflectionSelected, [this](core::shp_Reflection reflection) {
      setReflControls(reflection);
    });

    connect(&theHub, &TheHub::reflectionData, [this](core::shp_Reflection reflection) {
      setReflControls(reflection);
    });

    auto newReflData = [this](bool invalidateGuesses) {
      if (!silentSpin) {
        theHub.newReflectionData(
          core::Range::safeFrom(spinRangeMin->value(),spinRangeMax->value()),
          core::XY(spinGuessPeakX->value(),spinGuessPeakY->value()),
          spinGuessFwhm->value(), invalidateGuesses);
      }
    };

    auto changeReflData0 = [newReflData](qreal) {
      newReflData(false);
    };

    auto changeReflData1 = [newReflData](qreal) {
      newReflData(true);
    };

    connect(spinRangeMin,   static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData1);
    connect(spinRangeMax,   static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData1);
    connect(spinGuessPeakX, static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData0);
    connect(spinGuessPeakY, static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData0);
    connect(spinGuessFwhm,  static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),changeReflData0);
  }

  connect(this, &thisClass::currentChanged, [this](int index) {
    theHub.setFittingTab(index);
  });
}

void Fitting::enableReflControls(bool on) {
  spinRangeMin->setEnabled(on);
  spinRangeMax->setEnabled(on);
  spinGuessPeakX->setEnabled(on);
  spinGuessPeakY->setEnabled(on);
  spinGuessFwhm->setEnabled(on);
  readFitPeakX->setEnabled(on);
  readFitPeakY->setEnabled(on);
  readFitFwhm->setEnabled(on);
}

// TODO move to core_types ?

static qreal safeReal(qreal val) {
  return qIsFinite(val) ? val : 0.0;
}

static str safeRealText(qreal val) {
  return qIsFinite(val) ? str("%1").arg(val) : EMPTY_STR;
}

void Fitting::setReflControls(core::shp_Reflection const& reflection) {
  silentSpin = true;

  if (reflection.isNull()) {
    // do not set comboReflType - we want it to stay as it is
    spinRangeMin->setValue(0);
    spinRangeMax->setValue(0);
    spinGuessPeakX->setValue(0);
    spinGuessPeakY->setValue(0);
    spinGuessFwhm->setValue(0);
    readFitPeakX->clear();
    readFitPeakY->clear();
    readFitFwhm->clear();
  } else {
    comboReflType->setCurrentIndex((int)reflection->getType());

    auto &range = reflection->getRange();
    spinRangeMin->setValue(safeReal(range.min));
    spinRangeMax->setValue(safeReal(range.max));

    auto &peakFun = reflection->getPeakFunction();
    auto &guessPeak = peakFun.getGuessPeak();
    spinGuessPeakX->setValue(safeReal(guessPeak.x));
    spinGuessPeakY->setValue(safeReal(guessPeak.y));
    spinGuessFwhm->setValue(safeReal(peakFun.getGuessFWHM()));

    auto fitPeak = peakFun.getFitPeak();
    readFitPeakX->setText(safeRealText(fitPeak.x));
    readFitPeakY->setText(safeRealText(fitPeak.y));
    readFitFwhm->setText(safeRealText(peakFun.getFitFWHM()));
  }

  silentSpin = false;
}

void Fitting::updateReflectionControls() {
  reflectionView->hasReflections() ? enableReflControls(true) : enableReflControls(false);
}

//------------------------------------------------------------------------------
}
// eof

#include "fitting.h"
#include "thehub.h"
#include "core_fit_limits.h"
#include <QStyledItemDelegate>
#include <QAction>
#include <QApplication>

namespace panel {
//------------------------------------------------------------------------------

ReflectionView::ReflectionView(TheHub& theHub_)
: theHub(theHub_), model(theHub.reflectionViewModel) {
  setModel(&model);
  for_i (model.columnCount())
    resizeColumnToContents(i);
}

void ReflectionView::addReflection() {
  model.addReflection();
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
  theHub.actReflectionRegion->setEnabled(hasReflections());
  if (!hasReflections())
    theHub.actReflectionRegion->setChecked(false);
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
: super(EMPTY_STR,theHub_,Qt::Vertical), silentSpin(false) {

  box->addWidget(label("Background polynomial"));
  auto hb = hbox();
  box->addLayout(hb);

  hb->addWidget(iconButton(theHub.actBackgroundBackground));
  hb->addWidget(iconButton(theHub.actBackgroundClear));
  hb->addSpacing(65);
  hb->addWidget(label("Degree:"));
  hb->addWidget((spinDegree = spinCell(4,0,core::fit::MAX_BACKGROUND_POLYNOMIAL_DEGREE)));
  hb->addWidget(iconButton(theHub.actBackgroundShowFit));
  hb->addStretch();

  connect(spinDegree, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int degree) {
    theHub.setBackgroundPolynomialDegree(degree);
  });

  box->addWidget(label("Reflections"));
  auto hs = hbox();
  box->addLayout(hs);

  hs->addWidget(iconButton(theHub.actReflectionRegion));
  hs->addWidget(iconButton(theHub.actReflectionPeak));
  hs->addWidget(iconButton(theHub.actReflectionWidth));
  hs->addStretch();

  box->addWidget((reflectionView = new ReflectionView(theHub)));

  auto hr = hbox();
  box->addLayout(hr);

  hr->addWidget((comboReflType = comboBox(core::Reflection::reflTypes())));
  hr->addStretch();
  hr->addWidget(iconButton(theHub.actReflectionAdd));
  hr->addWidget(iconButton(theHub.actReflectionRemove));

  auto vb = vbox();
  box->addLayout(vb);

  auto gb = gridLayout();
  vb->addLayout(gb);

  gb->addWidget(label("min"),                     0, 0);
  gb->addWidget((spinRangeMin = spinCell(6,.0)),  0, 1);
  gb->addWidget(label("max"),                     0, 2);
  gb->addWidget((spinRangeMax = spinCell(6,.0)),  0, 3);

  gb->addWidget(label("peak x"),                  1, 0);
  gb->addWidget((spinPeakX    = spinCell(6,.0)),  1, 1);
  gb->addWidget(label("y"),                       1, 2);
  gb->addWidget((spinPeakY    = spinCell(6,.0)),  1, 3);

  gb->addWidget(label("fwhm"),                    2, 0);
  gb->addWidget((spinFwhm     = spinCell(6,.0)),  2, 1);

  gb->setColumnStretch(4,1);

  enableReflControls(false);

  connect(theHub.actReflectionAdd, &QAction::triggered, [this]() {
    reflectionView->addReflection();
    enableReflControls(true);
  });

  connect(theHub.actReflectionRemove, &QAction::triggered, [this]() {
    reflectionView->removeSelected();
    enableReflControls(false);
  });

  connect(&theHub, &TheHub::reflectionsChanged, [this]() {
    reflectionView->update();
  });

  connect(comboReflType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
    theHub.setReflType((core::Reflection::eType)index);
  });

  connect(&theHub, &TheHub::reflectionSelected, [this](core::shp_Reflection reflection) {
    setReflControls(reflection);
  });

  connect(&theHub, &TheHub::reflectionData, [this](core::shp_Reflection reflection) {
    setReflControls(reflection);
  });

  auto newReflData = [this](qreal) {
    if (!silentSpin) {
      theHub.newReflectionData(
        core::Range::safeFrom(spinRangeMin->value(),spinRangeMax->value()),
        core::XY(spinPeakX->value(),spinPeakY->value()),
        spinFwhm->value());
    }
  };

  connect(spinRangeMin, static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),newReflData);
  connect(spinRangeMax, static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),newReflData);
  connect(spinPeakX,    static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),newReflData);
  connect(spinPeakY,    static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),newReflData);
  connect(spinFwhm,     static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),newReflData);
}

void Fitting::enableReflControls(bool on) {
  spinRangeMin->setEnabled(on);
  spinRangeMax->setEnabled(on);
  spinPeakX->setEnabled(on);
  spinPeakY->setEnabled(on);
  spinFwhm->setEnabled(on);
}

// TODO move to core_types ?

static qreal safeReal(qreal val) {
  return qIsFinite(val) ? val : 0.0;
}

void Fitting::setReflControls(core::shp_Reflection const& reflection) {
  silentSpin = true;

  if (reflection.isNull()) {
    // do not set comboReflType - we want it to stay as it is
    spinRangeMin->setValue(0);
    spinRangeMax->setValue(0);
    spinPeakX->setValue(0);
    spinPeakY->setValue(0);
    spinFwhm->setValue(0);
  } else {
    comboReflType->setCurrentIndex(reflection->getType());
    auto range = reflection->getRange();
    spinRangeMin->setValue(safeReal(range.min));
    spinRangeMax->setValue(safeReal(range.max));
    spinPeakX->setValue(safeReal(reflection->getPeak().x));
    spinPeakY->setValue(safeReal(reflection->getPeak().y));
    spinFwhm->setValue(safeReal(reflection->getFWHM()));
  }

  silentSpin = false;
}

//------------------------------------------------------------------------------
}
// eof

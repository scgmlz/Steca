#include "fitting.h"
#include "thehub.h"
#include "core_fit_limits.h"
#include "core_reflection.h"
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

void ReflectionView::update() {
  model.signalReset();
  auto index = currentIndex();
  // keep the current index, or select the last item
  setCurrentIndex(index.isValid()
    ? index
    : model.index(qMax(0,model.rowCount()-1),0));

  bool hasReflections = model.rowCount() > 0;
  theHub.actReflectionRemove->setEnabled(hasReflections);
  theHub.actReflectionRegion->setEnabled(hasReflections);
  if (!hasReflections)
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
: super(EMPTY_STR,theHub_,Qt::Vertical) {

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

  auto rb = hbox();
  vb->addLayout(rb);

  rb->addWidget(label("min"));
  rb->addWidget((spinRangeMin = spinCell(6,.0)));
  rb->addWidget(label("max"));
  rb->addWidget((spinRangeMax = spinCell(6,.0)));
  rb->addStretch();

  connect(theHub.actReflectionAdd, &QAction::triggered, [this]() {
    reflectionView->addReflection();
  });

  connect(theHub.actReflectionRemove, &QAction::triggered, [this]() {
    reflectionView->removeSelected();
  });

  connect(&theHub, &TheHub::reflectionsChanged, [this]() {
    reflectionView->update();
  });

  connect(comboReflType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
    theHub.setReflType((core::Reflection::eType)index);
  });
}

//------------------------------------------------------------------------------
}
// eof

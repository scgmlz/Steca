#include "fitting.h"
#include "thehub.h"
#include "delegates.h"
#include "core_fit_limits.h"
#include <QAction>

namespace panel {
//------------------------------------------------------------------------------

ReflectionView::ReflectionView(TheHub& theHub_)
: theHub(theHub_), model(theHub.reflectionViewModel) {
  setModel(&model);
  setItemDelegateForColumn(2,new ComboBoxDelegate);

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

  uint row = index.row();
  index = ((int)row < model.rowCount()) ? index : index.sibling(row-1,0);

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

  theHub.actReflectionRemove->setEnabled(model.rowCount() > 0);
}

void ReflectionView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);
//  auto indexes = selected.indexes();
//  model.setSelectedDataset(indexes.isEmpty()
//    ? core::shp_Dataset()
//    : model.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Dataset>());
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

  hs->addWidget(iconButton(theHub.actSelectPeak));
  hs->addWidget(iconButton(theHub.actReflectionPeak));
  hs->addWidget(iconButton(theHub.actReflectionWidth));
  hs->addStretch();

  box->addWidget((reflectionView = new ReflectionView(theHub)));

  auto hr = hbox();
  box->addLayout(hr);
  hr->addStretch();
  hr->addWidget(iconButton(theHub.actReflectionAdd));
  hr->addWidget(iconButton(theHub.actReflectionRemove));

  auto vb = vbox();
  box->addLayout(vb);
  vb->addWidget(label("info1"));
  vb->addWidget(label("info2"));
  vb->addWidget(label("info3"));

  connect(theHub.actReflectionAdd, &QAction::triggered, [this]() {
    reflectionView->addReflection();
  });

  connect(theHub.actReflectionRemove, &QAction::triggered, [this]() {
    reflectionView->removeSelected();
  });
}

//------------------------------------------------------------------------------
}
// eof

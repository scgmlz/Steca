#include "fitting.h"
#include "mainwin.h"
#include "session.h"
#include "delegates.h"
#include "core_fit_limits.h"
#include <QAction>

namespace panel {
//------------------------------------------------------------------------------

ReflectionView::ReflectionView(Fitting& fitting_,Model& model_)
: fitting(fitting_), model(model_) {
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

  fitting.mainWin.actReflectionRemove->setEnabled(model.rowCount() > 0);
}

void ReflectionView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);
//  auto indexes = selected.indexes();
//  model.setSelectedDataset(indexes.isEmpty()
//    ? core::shp_Dataset()
//    : model.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Dataset>());
}

//------------------------------------------------------------------------------

Fitting::Fitting(MainWin& mainWin,Session& session)
: super(EMPTY_STR,mainWin,session,Qt::Vertical) {

  box->addWidget(label("Background polynomial"));
  auto hb = hbox();
  box->addLayout(hb);

  hb->addWidget(iconButton(mainWin.actBackgroundBackground));
  hb->addWidget(label("Degree:"));
  hb->addWidget((spinDegree = spinCell(4,0,core::fit::MAX_BACKGROUND_POLYNOMIAL_DEGREE)));
  hb->addWidget(iconButton(mainWin.actBackgroundShowFit));
  hb->addWidget(iconButton(mainWin.actBackgroundClear));
  hb->addStretch();

  connect(spinDegree, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int degree) {
    getSession().setBackgroundPolynomialDegree(degree);
  });

  box->addWidget(label("Reflections"));
  box->addWidget((reflectionView = new ReflectionView(*this,session.reflectionViewModel)));

  auto hr = hbox();
  box->addLayout(hr);

  hr->addWidget(iconButton(mainWin.actSelectPeak));
  hr->addWidget(iconButton(mainWin.actReflectionPeak));
  hr->addWidget(iconButton(mainWin.actReflectionWidth));
  hr->addStretch();
  hr->addWidget(iconButton(mainWin.actReflectionAdd));
  hr->addWidget(iconButton(mainWin.actReflectionRemove));

  auto vb = vbox();
  box->addLayout(vb);
  vb->addWidget(label("info1"));
  vb->addWidget(label("info2"));
  vb->addWidget(label("info3"));

  connect(mainWin.actReflectionAdd, &QAction::triggered, [this]() {
    reflectionView->addReflection();
  });

  connect(mainWin.actReflectionRemove, &QAction::triggered, [this]() {
    reflectionView->removeSelected();
  });
}

//------------------------------------------------------------------------------
}
// eof

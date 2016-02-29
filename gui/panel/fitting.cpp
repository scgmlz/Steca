#include "fitting.h"
#include "mainwin.h"
#include "session.h"
#include "core_fit_limits.h"
#include <QAction>

namespace panel {
//------------------------------------------------------------------------------

ReflectionView::ReflectionView(Model& model_): model(model_) {
//  TODO
//  setColumnCount(6);
//  hideColumn(0);
//  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//  header()->hide();
//  setItemDelegateForColumn(1,new IconDelegate(":/icon/integral",true));
//  setItemDelegateForColumn(2,new RadioDelegate);
//  setItemDelegateForColumn(3,new ComboBoxDelegate);
//  setItemDelegateForColumn(4,new IconDelegate(":/icon/eye",true));
  //  setItemDelegateForColumn(5,new IconDelegate(":/icon/rem",false));
}

void ReflectionView::addReflection() {
  model.addReflection();
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
  box->addWidget(reflectionView = new ReflectionView(session.reflectionViewModel));

  auto hr = hbox();
  box->addLayout(hr);

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
}

//------------------------------------------------------------------------------
}
// eof

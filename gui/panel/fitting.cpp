#include "fitting.h"
#include "mainwin.h"
#include "session.h"
#include "core_fit_limits.h"

namespace panel {
//------------------------------------------------------------------------------

ReflectionView::ReflectionView() {
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
  hb->addStretch();

  connect(spinDegree, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int degree) {
    getSession().setBackgroundPolynomialDegree(degree);
  });

  box->addWidget(label("Reflections"));
  box->addWidget(reflectionView = new ReflectionView());

  auto hr = hbox();
  box->addLayout(hr);

  hr->addWidget(iconButton(mainWin.actReflectionPeak));
  hr->addWidget(iconButton(mainWin.actReflectionReflect));
  hr->addWidget(iconButton(mainWin.actReflectionWidth));
  hr->addStretch();
  hr->addWidget(iconButton(mainWin.actReflectionAdd));

  auto vb = vbox();
  box->addLayout(vb);
  vb->addWidget(label("info1"));
  vb->addWidget(label("info2"));
  vb->addWidget(label("info3"));
}

//------------------------------------------------------------------------------
}
// eof

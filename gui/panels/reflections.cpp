#include "reflections.h"
#include "mainwin.h"

namespace panel {

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

//-----------------------------------------------------------------------------

Reflections::Reflections(MainWin& mainWin): super(mainWin,"Reflections",Qt::Vertical) {
  auto h = hbox();
  box->addLayout(h);

  h->addWidget(label("Output:"));
  h->addWidget(iconButton(mainWin.actReflectionPeak));
  h->addWidget(iconButton(mainWin.actReflectionReflect));
  h->addWidget(iconButton(mainWin.actReflectionWidth));
  h->addStretch();

  box->addWidget(reflectionView = new ReflectionView());

  h = hbox();
  box->addLayout(h);
  h->addStretch();
  h->addWidget(iconButton(mainWin.actReflectionAdd));
}

}

// eof

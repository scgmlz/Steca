#include "reflections.h"
#include "widgets/iconbutton.h"
#include "widgets/metrics.h"
#include "widgets/delegates.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QResizeEvent>

ReflectionList::ReflectionList() {
  setMaximumWidth(textWidth("mmmmmmmmmmmmmm"));
  setColumnCount(6);
  hideColumn(0);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  header()->hide();
  setItemDelegateForColumn(1,new IconDelegate(":/icon/integral",true));
  setItemDelegateForColumn(2,new RadioDelegate);
  setItemDelegateForColumn(3,new ComboBoxDelegate);
  setItemDelegateForColumn(4,new IconDelegate(":/icon/eye",true));
  setItemDelegateForColumn(5,new IconDelegate(":/icon/remove",false));
}

void ReflectionList::resizeEvent(QResizeEvent* e) {
  int w = e->size().width();
  setColumnWidth(1,20);
  setColumnWidth(2,20);
  setColumnWidth(3,w-80);
  setColumnWidth(4,20);
  setColumnWidth(5,20);
}

ReflectionItem::ReflectionItem(): super() {
}

Reflections::Reflections(): super("Reflections") {
  auto sp = sizePolicy();
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  auto h = new HBox;
  v->addLayout(h);

  h->addWidget(new QLabel("Output:"));
  h->addWidget(new IconButton(":/icon/peak"));
  h->addWidget(new IconButton(":/icon/reflect"));
  h->addWidget(new IconButton(":/icon/width"));
  h->addStretch();

  v->addWidget(l = new ReflectionList());

  l->addTopLevelItem(new ReflectionItem());
  l->addTopLevelItem(new ReflectionItem());
  l->addTopLevelItem(new ReflectionItem());
  l->addTopLevelItem(new ReflectionItem());

  h = new HBox;
  v->addLayout(h);
  h->addStretch();
  h->addWidget(new IconButton(":/icon/add"));
}

// eof

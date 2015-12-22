#include "files.h"
#include "widgets/metrics.h"
#include "widgets/iconbutton.h"
#include "widgets/delegates.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QResizeEvent>
#include <QPushButton>

FileList::FileList() {
  setMaximumWidth(textWidth("mmmmmmmmmmmmmm"));
  setColumnCount(3);
  hideColumn(0);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  header()->hide();
  setItemDelegateForColumn(2,new IconDelegate(":/icon/remove",false));
}

void FileList::resizeEvent(QResizeEvent* e) {
  int w = e->size().width();
  setColumnWidth(1,w-20);
  setColumnWidth(2,20);
}

FileItem::FileItem(rcstr s): super() {
  setText(1,s);
}

Files::Files(): super("Files") {
  auto sp = sizePolicy();
  sp.setVerticalStretch(1);
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  v->addWidget(l = new FileList());
  FileItem *fi;
  l->addTopLevelItem((fi=new FileItem("m0001")));
  l->addTopLevelItem(new FileItem("m0002"));
  l->addTopLevelItem(new FileItem(""));
  l->addTopLevelItem(new FileItem("corr-m    Corr."));
  l->setCurrentItem(fi);

  auto h = new HBox;
  v->addLayout(h);
  h->addWidget(new QPushButton("Combine..."));
  h->addWidget(new IconButton(":/icon/add"));
}

// eof

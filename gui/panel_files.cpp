#include "panel_files.h"
#include "layout_helpers.h"

#include <QTreeWidget>
#include <QHeaderView>

class xFileList: public QTreeWidget {
  SUPER(xFileList,QTreeWidget)
public:
  xFileList();

protected:
  void resizeEvent(QResizeEvent*);
};

xFileList::xFileList() {
//  setMaximumWidth(textWidth("mmmmmmmmmmmmmm"));
//  setColumnCount(3);
//  hideColumn(0);
//  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//  header()->hide();
//  setItemDelegateForColumn(2,new IconDelegate(":/icon/remove",false));
}

void xFileList::resizeEvent(QResizeEvent*) {
//  int w = e->size().width();
//  setColumnWidth(1,w-20);
//  setColumnWidth(2,20);
}

PanelFiles::PanelFiles() {
  v->addWidget(new xFileList);
}

// eof

#include "panel_files.h"
#include "mainwin.h"
#include "layout_helpers.h"

#include <QListView>
#include <QHeaderView>

class FileList: public QListView {
  SUPER(FileList,QListView)
public:
  FileList(MainWin&);

protected:
  void resizeEvent(QResizeEvent*);
};

FileList::FileList(MainWin&) {
//  setMaximumWidth(textWidth("mmmmmmmmmmmmmm"));
//  setColumnCount(3);
//  hideColumn(0);
//  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//  header()->hide();
//  setItemDelegateForColumn(2,new IconDelegate(":/icon/remove",false));
}

void FileList::resizeEvent(QResizeEvent*) {
//  int w = e->size().width();
//  setColumnWidth(1,w-20);
//  setColumnWidth(2,20);
}

class FileModel: public QAbstractListModel {
  SUPER(FileModel,QAbstractListModel)
public:
  FileModel(Core&);

  int rowCount(QModelIndex const&)      const;
  QVariant data(QModelIndex const&,int) const;

private:
  Core &core;
};

FileModel::FileModel(Core& core_): core(core_) {
}

int FileModel::rowCount(QModelIndex const& parent) const {
  if (parent.isValid()) return 0;
  return core.numFiles();
}

QVariant FileModel::data(QModelIndex const& index,int role) const {
  auto row = index.row();
  if (row < 0 || row >= core.numFiles()) goto def;

  switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
      return core.fileName(row).data(); // converts std::string to QVariant
    default:
      goto def;
  }

def:
  return QVariant();
}

PanelFiles::PanelFiles(MainWin& mainWin): super(mainWin) {
  auto l = new FileList(mainWin);
  l->setModel(new FileModel(mainWin.getCore()));
  v->addWidget(l);
}

// eof

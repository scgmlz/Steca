#include "panel_files.h"
#include "mainwin.h"
#include "layout_helpers.h"

#include <QPainter>

//-----------------------------------------------------------------------------

FileList::FileList(Core& core): model(core), delegate(*this) {
  setModel(&model);
  setItemDelegate(&delegate);
}

void FileList::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  str fileName;

  auto indexes = selected.indexes();
  if (!indexes.isEmpty())
    fileName = model.data(indexes.first(),Qt::DisplayRole).toString();

  emit selectedFile(fileName);
}

void FileList::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
  super::currentChanged(current,previous);
}

//-----------------------------------------------------------------------------

// TODO see http://cep.xray.aps.anl.gov/software/qt4-x11-4.8.6-browser/d1/ded/qstringlistmodel_8cpp_source.html
FileList::Model::Model(Core& core_): core(core_) {
}

int FileList::Model::rowCount(QModelIndex const&) const {
  return core.numFiles() + (core.hasCorrectionFile() ? 1 : 0);
}

enum { IsCorrectionFileRole = Qt::UserRole };

QVariant FileList::Model::data(QModelIndex const& index,int role) const {
  auto row = index.row(), cnt = rowCount(index);
  if (row < 0 || row >= cnt) return QVariant();

  bool isCorrectionFile = core.hasCorrectionFile() && row+1 == cnt;

  switch (role) {
    case IsCorrectionFileRole:
      return isCorrectionFile;
    case Qt::DisplayRole: {
      auto fileName = core.fileName(row);
      char const* data = fileName.data();
      static str Corr("Corr: ");
      return isCorrectionFile ? Corr + data : data;
    }
    default:
      return QVariant();
  }
}

//-----------------------------------------------------------------------------

FileList::Delegate::Delegate(FileList& fileList_): fileList(fileList_) {
}

void FileList::Delegate::paint(QPainter* painter,
  QStyleOptionViewItem const& option, QModelIndex const& index) const
{
  bool isCorrectionFile = index.data(IsCorrectionFileRole).toBool();
  if(isCorrectionFile) {
    QStyleOptionViewItem o = option;
    auto &font = o.font;
    font.setItalic(true);
    font.setBold(true);
    super::paint(painter,o,index);
  } else {
    super::paint(painter,option,index);
  }
}

//-----------------------------------------------------------------------------

PanelFiles::PanelFiles(MainWin& mainWin): super(mainWin) {
  v->addWidget((fileList = new FileList(mainWin.core)));

  auto h = hbox(); v->addLayout(h);

  btnAddCorrectionFile  = new Button("Corr. file...","add or replace correction file");
  btnAddFile            = new IconButton(":/icon/add","add file");
  btnRemoveFile         = new IconButton(":/icon/remove","remove selected file");

  btnRemoveFile->setEnabled(false);

  h->addWidget(btnAddCorrectionFile);
  h->addStretch();
  h->addWidget(btnAddFile);
  h->addWidget(btnRemoveFile);

  connect(fileList, &FileList::selectedFile, [&](rcstr s) {
    btnRemoveFile->setEnabled(!s.isEmpty());
  });
}

//-----------------------------------------------------------------------------

// eof

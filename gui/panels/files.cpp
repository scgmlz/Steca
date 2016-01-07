#include "files.h"
#include "app.h"
#include "mainwin.h"
#include "gui_helpers.h"

#include <QAction>
#include <QPainter>

namespace panel {

//-----------------------------------------------------------------------------

FileList::FileList(Session& session): model(session) {
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

void FileList::removeSelectedFile() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  uint row = index.row();
  index = (row+1 < model.session.numFiles(true))
    ? index : index.sibling(row-1,0);
  model.session.remFile(row);
  emit selectedFile(str::null);
  setCurrentIndex(index);
}

//-----------------------------------------------------------------------------

FileList::Model::Model(Session& session_): session(session_) {
}

int FileList::Model::rowCount(QModelIndex const&) const {
  return session.numFiles(true);
}

enum { IsCorrectionFileRole = Qt::UserRole, GetFileRole };

QVariant FileList::Model::data(QModelIndex const& index,int role) const {
  auto row = index.row(), cnt = rowCount(index);
  if (row < 0 || row >= cnt) return QVariant();

  bool isCorrectionFile = session.hasCorrFile() && row+1 == cnt;

  switch (role) {
    case IsCorrectionFileRole:
      return isCorrectionFile;
    case Qt::DisplayRole: {
      str s = session.fileName(row);
      static str Corr("Corr: ");
      if (isCorrectionFile) s = Corr + s;
      return s;
    }
    case GetFileRole:
      return &(session.getFile(row));
    default:
      return QVariant();
  }
}

//-----------------------------------------------------------------------------

FileList::Delegate::Delegate() {
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

Files::Files(MainWin& mainWin): super(mainWin,"Files",Qt::Vertical) {
  box->addWidget((fileList = new FileList(mainWin.session)));

  auto h = hbox(); box->addLayout(h);

  h->addWidget(textButton(mainWin.actSetCorrectionFile));
  h->addStretch();
  h->addWidget(iconButton(mainWin.actAddFiles));
  h->addWidget(iconButton(mainWin.actRemoveFile));

  mainWin.actRemoveFile->setEnabled(false);


  connect(fileList, &FileList::selectedFile, [&](rcstr s) {
    mainWin.actRemoveFile->setEnabled(!s.isEmpty());
  });

  connect(mainWin.actRemoveFile, &QAction::triggered, [&]() {
    fileList->removeSelectedFile();
  });

  connect(&mainWin.session, &Session::filesChanged, [&]() {
    fileList->reset();
  });
}

}

// eof

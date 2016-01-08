#include "files.h"
#include "app.h"
#include "mainwin.h"
#include "gui_helpers.h"

#include <QAction>
#include <QPainter>

namespace panel {

//-----------------------------------------------------------------------------

FileList::FileList(Session& session_): session(session_) {
  setModel(&session.fileListModel);
  setItemDelegate(&delegate);
}

void FileList::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto& model  = session.fileListModel;

  auto indexes = selected.indexes();
  QVariant v = model.data(indexes.first(), Session::FileListModel::GetFileRole);
  auto p = v.value<pcCoreFile>();
  model.session.emitSelectedFile(indexes.isEmpty()
                                 ? nullptr
                                 : model.data(indexes.first(), Session::FileListModel::GetFileRole).value<pcCoreFile>());
}

void FileList::removeSelectedFile() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  auto& model  = session.fileListModel;

  uint row = index.row();
  index = (row+1 < model.session.numFiles(true))
    ? index : index.sibling(row-1,0);
  model.session.remFile(row);
  model.session.emitSelectedFile(nullptr);
  setCurrentIndex(index);
}

//-----------------------------------------------------------------------------

FileList::Delegate::Delegate() {
}

void FileList::Delegate::paint(QPainter* painter,
  QStyleOptionViewItem const& option, QModelIndex const& index) const
{
  bool isCorrectionFile = index.data(Session::FileListModel::IsCorrectionFileRole).toBool();
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

  connect(mainWin.actRemoveFile, &QAction::triggered, [&]() {
    fileList->removeSelectedFile();
  });

  connect(&mainWin.session, &Session::filesChanged, [&]() {
    fileList->reset();
  });
}

}

// eof

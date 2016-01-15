#include "files.h"
#include "app.h"
#include "mainwin.h"
#include "gui_helpers.h"

#include <QAction>
#include <QPainter>

namespace panel {

//-----------------------------------------------------------------------------

FileView::FileView(Session& session_): session(session_) {
  setModel(&session.fileViewModel);
  setItemDelegate(&delegate);
}

void FileView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto& model  = session.fileViewModel;

  auto indexes = selected.indexes();
  model.session.setSelectedFile(indexes.isEmpty()
                                 ? nullptr
                                 : model.data(indexes.first(), Session::FileViewModel::GetFileRole).value<pcCoreFile>());
}

void FileView::removeSelectedFile() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  auto& model  = session.fileViewModel;

  uint row = index.row();
  index = (row+1 < model.session.numFiles(true)) ? index
                                                 : index.sibling(row-1,0);
  model.session.remFile(row);
  if (0>=model.session.numFiles(true))
    model.session.setSelectedFile(nullptr);
  setCurrentIndex(index);
}

//-----------------------------------------------------------------------------

FileView::Delegate::Delegate() {
}

void FileView::Delegate::paint(QPainter* painter,
  QStyleOptionViewItem const& option, QModelIndex const& index) const
{
  bool isCorrectionFile = index.data(Session::FileViewModel::IsCorrectionFileRole).toBool();
  if(isCorrectionFile) {
    QStyleOptionViewItem o = option;
    auto &font = o.font;
    font.setItalic(true);
    super::paint(painter,o,index);
  } else {
    super::paint(painter,option,index);
  }
}

//-----------------------------------------------------------------------------

Files::Files(MainWin& mainWin_): super(mainWin_,"Files",Qt::Vertical) {
  box->addWidget((fileView = new FileView(mainWin.session)));

  auto h = hbox(); box->addLayout(h);

  h->addWidget(textButton(mainWin.actSetCorrectionFile));
  h->addStretch();
  h->addWidget(iconButton(mainWin.actAddFiles));
  h->addWidget(iconButton(mainWin.actRemoveFile));

  connect(mainWin.actRemoveFile, &QAction::triggered, [this]() {
    fileView->removeSelectedFile();
  });

  connect(&mainWin.session, &Session::filesChanged, [this]() {
    fileView->reset();
    fileView->setCurrentIndex(mainWin.session.fileViewModel.index(0)); // TODO untangle
  });
}

}

// eof

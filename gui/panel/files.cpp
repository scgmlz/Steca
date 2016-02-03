#include "files.h"
#include "mainwin.h"
#include "session.h"
#include "models.h"
#include <QStyledItemDelegate>
#include <QAction>

namespace panel {

//-----------------------------------------------------------------------------

class FileViewDelegate: public QStyledItemDelegate {
  SUPER(FileViewDelegate,QStyledItemDelegate)
public:
  void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const {
    QStyleOptionViewItem o = option;
    bool isCorrectionFile = index.data(FileView::Model::IsCorrectionFileRole).toBool();
    if(isCorrectionFile)
      o.font.setItalic(true);
    super::paint(painter,o,index);
  }
};

FileView::FileView(Model& model_): model(model_) {
  setModel(&model);

  static FileViewDelegate delegate;
  setItemDelegate(&delegate);
}

void FileView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  model.setSelectedFile(indexes.isEmpty()
    ? core::shp_File()
    : model.data(indexes.first(), Model::GetFileRole).value<core::shp_File>());
}

void FileView::removeSelectedFile() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  uint row = index.row();
  index = (row+1 < model.numFiles(true)) ? index : index.sibling(row-1,0);
  model.remFile(row);
  if (0>=model.numFiles(true))
    model.setSelectedFile(core::shp_File());
  setCurrentIndex(index);
}

//-----------------------------------------------------------------------------

Files::Files(MainWin& mainWin_,Session& session_)
: super("Files",mainWin_,session_,Qt::Vertical) {
  box->addWidget((fileView = new FileView(session.fileViewModel)));

  auto h = hbox(); box->addLayout(h);

  h->addWidget(textButton(mainWin.actLoadCorrectionFile));
  h->addStretch();
  h->addWidget(iconButton(mainWin.actAddFiles));
  h->addWidget(iconButton(mainWin.actRemoveFile));

  connect(mainWin.actRemoveFile, &QAction::triggered, [this]() {
    fileView->removeSelectedFile();
  });

  connect(&session, &Session::filesChanged, [this]() {
    fileView->reset();
    fileView->setCurrentIndex(session.fileViewModel.index(0)); // TODO untangle
  });
}

}

// eof

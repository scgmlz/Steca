// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_file.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "panel_file.h"
#include "thehub.h"
#include "models.h"
#include <QStyledItemDelegate>
#include <QHeaderView>

namespace panel {
//-----------------------------------------------------------------------------

class FileViewDelegate: public QStyledItemDelegate {
  SUPER(FileViewDelegate,QStyledItemDelegate)
public:
  void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex index) const {
    QStyleOptionViewItem o = option;
    bool isCorrFile = index.data(FileView::Model::IsCorrFileRole).toBool();
    if (isCorrFile) {
      o.font.setItalic(true);
      o.font.setBold(true);
    }
    super::paint(painter,o,index);
  }
};

//------------------------------------------------------------------------------

FileView::FileView(TheHub& theHub): super(theHub), model(theHub.fileViewModel) {
  setModel(&model);

  header()->hide();

  static FileViewDelegate delegate;
  setItemDelegate(&delegate);
}

void FileView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  theHub.setSelectedFile(indexes.isEmpty()
    ? core::shp_File()
    : model.data(indexes.first(), Model::GetFileRole).value<core::shp_File>());
}

void FileView::removeSelected() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  uint row = index.row();
  index = ((int)(row+1) < model.rowCount()) ? index : index.sibling(row-1,0);

  model.remFile(row);
  if (0>=model.rowCount()) // no more files
    theHub.setSelectedFile(core::shp_File());

  setCurrentIndex(index);
}

void FileView::update() {
  auto index = currentIndex();
  model.signalReset();
  // keep the current index, or select the first item
  setCurrentIndex(index.isValid() ? index : model.index(0,1));
}

//------------------------------------------------------------------------------

DockFiles::DockFiles(TheHub& theHub)
: super("Files","dock-files",Qt::Vertical) {
  box->addWidget((fileView = new FileView(theHub)));

  auto h = hbox(); box->addLayout(h);

  h->addWidget(textButton(theHub.actLoadCorrFile));
  h->addWidget(iconButton(theHub.actImagesEnableCorr));
  h->addStretch();
  h->addWidget(iconButton(theHub.actAddFiles));
  h->addWidget(iconButton(theHub.actRemoveFile));

  connect(theHub.actRemoveFile, &QAction::triggered, [this]() {
    fileView->removeSelected();
  });

  connect(&theHub, &TheHub::filesChanged, [this]() {
    fileView->update();
  });
}

//------------------------------------------------------------------------------
}
// eof

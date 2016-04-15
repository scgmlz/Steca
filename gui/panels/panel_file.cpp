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

namespace gui { namespace panel {
//-----------------------------------------------------------------------------

class FileViewDelegate: public QStyledItemDelegate {
  SUPER(FileViewDelegate,QStyledItemDelegate)
public:
  void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const {
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

FileView::FileView(TheHub& theHub): super(theHub), model_(theHub.fileViewModel) {
  setModel(&model_);

  header()->hide();

  static FileViewDelegate delegate;
  setItemDelegate(&delegate);
}

void FileView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  theHub_.setSelectedFile(indexes.isEmpty()
    ? core::shp_File()
    : model_.data(indexes.first(), Model::GetFileRole).value<core::shp_File>());
}

void FileView::removeSelected() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  uint row = index.row();
  index = ((int)(row+1) < model_.rowCount()) ? index : index.sibling(row-1,0);

  model_.remFile(row);
  if (0>=model_.rowCount()) // no more files
    theHub_.setSelectedFile(core::shp_File());

  setCurrentIndex(index);
}

void FileView::update() {
  auto index = currentIndex();
  model_.signalReset();
  // keep the current index, or select the first item
  setCurrentIndex(index.isValid() ? index : model_.index(0,1));
}

//------------------------------------------------------------------------------

DockFiles::DockFiles(TheHub& theHub)
: super("Files","dock-files",Qt::Vertical), RefHub(theHub) {
  box_->addWidget((fileView_ = new FileView(theHub)));

  auto h = hbox(); box_->addLayout(h);

  auto &actions = theHub.actions;

  h->addWidget(textButton(actions.loadCorrFile));
  h->addWidget(iconButton(actions.enableCorr));
  h->addStretch();
  h->addWidget(iconButton(actions.addFiles));
  h->addWidget(iconButton(actions.remFile));

  connect(actions.remFile, &QAction::triggered, [this]() {
    fileView_->removeSelected();
  });

  connect(&theHub_, &TheHub::filesChanged, [this]() {
    fileView_->update();
  });
}

//------------------------------------------------------------------------------
}}
// eof

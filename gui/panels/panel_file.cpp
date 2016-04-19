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
//------------------------------------------------------------------------------

FilesView::FilesView(TheHub& hub): super(hub), model_(hub.fileViewModel) {
  setModel(&model_);
  header()->hide();
}

void FilesView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  hub_.setSelectedFile(indexes.isEmpty()
    ? core::shp_File()
    : model_.data(indexes.first(), Model::GetFileRole).value<core::shp_File>());
}

void FilesView::removeSelected() {
  int row = currentIndex().row();
  if (row<0 || model_.rowCount() <= row) return;

  model_.remFile(row);
  if (!model_.rowCount()) // no more files
    hub_.setSelectedFile(core::shp_File());

  update();
}

void FilesView::update() {
  super::update(model_);
}

//------------------------------------------------------------------------------

DockFiles::DockFiles(TheHub& hub)
: super("Files","dock-files",Qt::Vertical), RefHub(hub) {
  box_->addWidget((filesView_ = new FilesView(hub)));

  auto h = hbox(); box_->addLayout(h);

  auto &actions = hub_.actions;

  h->addWidget(label("Corr. file"));
  h->addStretch();
  h->addWidget(iconButton(actions.addFiles));
  h->addWidget(iconButton(actions.remFile));

  h = hbox(); box_->addLayout(h);

  h->addWidget((corrFile_  = new LineView()));
  h->addWidget(iconButton(actions.enableCorr));
  h->addWidget(iconButton(actions.remCorr));

  connect(actions.remFile, &QAction::triggered, [this]() {
    filesView_->removeSelected();
  });

  connect(actions.remCorr, &QAction::triggered, [this]() {
    hub_.setCorrFile(EMPTY_STR);
  });

  connect(&hub_, &TheHub::filesChanged, [this]() {
    filesView_->update();
  });

  connect(&hub_, &TheHub::corrFileName, [this](rcstr fileName) {
    corrFile_->setText(fileName);
    hub_.actions.remCorr->setEnabled(!fileName.isEmpty());
  });
}

//------------------------------------------------------------------------------
}}
// eof

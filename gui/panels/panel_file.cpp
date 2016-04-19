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
  setSelectionMode(ExtendedSelection);
  header()->hide();
}

void FilesView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto &wds = hub_.workingDatasets();
  wds.clear();

  auto indexes = selectedIndexes(); uint cnt = 0;
  for (auto &index: indexes) {
    auto file = model_.data(index, Model::GetFileRole).value<core::shp_File>();
    for (auto &ds: file->datasets()) {
      wds.appendHere(ds);
      ++cnt;
    }
  }

  hub_.setFilesSelectedDatasetsChanged(!indexes.isEmpty(),cnt);
}

void FilesView::removeSelected() {
  auto indexes = selectedIndexes();

  // then delete backwards
  for (uint i = indexes.count(); i-- > 0; )
    model_.remFile(indexes.at(i).row());

  update();
}

void FilesView::update() {
  selectionModel()->clearSelection();
  model_.signalReset();
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

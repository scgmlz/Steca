/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "dock_files.h"
#include "thehub.h"
#include "views.h"
#include <QHeaderView>

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class FilesView : public views::MultiListView {
  CLS(FilesView) SUPER(views::MultiListView)
public:
  FilesView(TheHub&);

protected:
  using Model = models::FilesModel;
  Model* model() const { return static_cast<Model*>(super::model()); }

  void selectionChanged(QItemSelection const&, QItemSelection const&);
  void removeSelected();
  void recollect();
};

FilesView::FilesView(TheHub& hub) : super(hub) {
  setModel(&hub.filesModel);
  EXPECT(dynamic_cast<Model*>(super::model()))

  header()->hide();

  connect(hub_.actions.remFile, &QAction::triggered,
          [this]() { removeSelected(); });

  onSigFilesChanged([this]() {
    selectRows({});
    recollect();
  });

  onSigFilesSelected([this]() {
    selectRows(hub_.collectedFromFiles());
  });
}

void FilesView::selectionChanged(QItemSelection const& selected,
                                 QItemSelection const& deselected) {
  super::selectionChanged(selected, deselected);
  recollect();
}

void FilesView::removeSelected() {
  auto indexes = selectedIndexes();

  // backwards
  for (int i = indexes.count(); i-- > 0;)
    model()->remFile(to_u(indexes.at(i).row()));

  selectRows({});
  recollect();
}

void FilesView::recollect() {
  uint_vec rows;
  for (auto& index : selectionModel()->selectedRows())
    if (index.isValid())
      rows.append(to_u(index.row()));

  hub_.collectDatasetsFromFiles(rows);
}

//------------------------------------------------------------------------------

DockFiles::DockFiles(TheHub& hub)
: super("Files", "dock-files", Qt::Vertical), RefHub(hub)
{
  auto& actions = hub_.actions;

  auto h = hbox();
  box_->addLayout(h);

  h->addStretch();
  h->addWidget(iconButton(actions.addFiles));
  h->addWidget(iconButton(actions.remFile));

  box_->addWidget((filesView_ = new FilesView(hub)));

  h = hbox();
  box_->addLayout(h);

  h->addWidget(label("Correction file"));

  h = hbox();
  box_->addLayout(h);

  h->addWidget((corrFile_ = new LineView()));
  h->addWidget(iconButton(actions.enableCorr));
  h->addWidget(iconButton(actions.remCorr));

  onSigCorrFile([this](data::shp_File file) {
    corrFile_->setText(file.isNull() ? EMPTY_STR : file->fileName());
  });
}

//------------------------------------------------------------------------------
}}
// eof

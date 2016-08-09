// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      panel_file.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "panel_file.h"
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
  for (uint i = indexes.count(); i-- > 0;)
    model()->remFile(indexes.at(i).row());

  selectRows({});
  recollect();
}

void FilesView::recollect() {
  uint_vec rows;
  for (auto& index : selectionModel()->selectedRows())
    if (index.isValid()) rows.append(index.row());

  hub_.collectDatasetsFromFiles(rows);
}

//------------------------------------------------------------------------------

DockFiles::DockFiles(TheHub& hub)
: super("Files", "dock-files", Qt::Vertical), RefHub(hub)
{
  box_->addWidget((filesView_ = new FilesView(hub)));

  auto h = hbox();
  box_->addLayout(h);

  auto& actions = hub_.actions;

  h->addWidget(label("Corr. file"));
  h->addStretch();
  h->addWidget(iconButton(actions.addFiles));
  h->addWidget(iconButton(actions.remFile));

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

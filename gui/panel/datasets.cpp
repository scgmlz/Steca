#include "datasets.h"
#include "mainwin.h"
#include "session.h"
#include "models.h"

namespace panel {
//------------------------------------------------------------------------------

DatasetView::DatasetView(Model& model_): model(model_) {
  setModel(&model);

  connect(&model.getSession(), &Session::fileSelected, [this](core::shp_File coreFile) {
    model.setCoreFile(coreFile);
    setCurrentIndex(model.index(0,0));
  });

  connect(&model, &QAbstractItemModel::modelReset, [this]() {
    for_i (model.columnCount(QModelIndex()))
      resizeColumnToContents(i);
  });
}

void DatasetView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  model.setSelectedDataset(indexes.isEmpty()
    ? core::shp_Dataset()
    : model.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Dataset>());
}

//------------------------------------------------------------------------------

Datasets::Datasets(MainWin& mainWin_,Session& session_)
: super("Datasets",mainWin_,session_,Qt::Vertical) {
  box->addWidget((datasetView = new DatasetView(session.datasetViewModel)));
  auto h = hbox();
  box->addLayout(h);
  h->addWidget(label("Combine:"));
  h->addWidget(spinCell(4,1));
  h->addStretch();
}

//------------------------------------------------------------------------------
}
// eof

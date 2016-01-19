#include "datasets.h"
#include "mainwin.h"

namespace panel {

DatasetView::DatasetView(Session& session_): session(session_) {
  setModel(&session.datasetViewModel);

  connect(&session, &Session::fileSelected, [this](pcCoreFile coreFile) {
    auto& model  = session.datasetViewModel;
    model.setCoreFile(coreFile);
    setCurrentIndex(model.index(0,0));
  });

  connect(model(), &QAbstractItemModel::modelReset, [this]() {
    for_i(model()->columnCount())
      resizeColumnToContents(i);
  });
}

void DatasetView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  session.setSelectedDataset(indexes.isEmpty()
    ? nullptr
    : session.datasetViewModel.data(indexes.first(),
        Session::DatasetViewModel::GetDatasetRole).value<pcCoreDataset>());
}

//------------------------------------------------------------------------------

Datasets::Datasets(MainWin& mainWin_): super(mainWin_,"Datasets",Qt::Vertical) {
  box->addWidget((datasetView = new DatasetView(mainWin.session)));
  auto h = hbox();
  box->addLayout(h);
  h->addWidget(label("Combine:"));
  h->addWidget(spinCell(4,1));
  h->addStretch();
}

}

// eof

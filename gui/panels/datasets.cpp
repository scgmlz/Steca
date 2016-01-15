#include "datasets.h"
#include "mainwin.h"

namespace panel {

DatasetView::DatasetView(Session& session_): session(session_) {
  setModel(&session.datasetViewModel);
}

void DatasetView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto& model  = session.datasetViewModel;

  auto indexes = selected.indexes();
  model.session.setSelectedDataset(indexes.isEmpty()
    ? nullptr
    : model.data(indexes.first(), Session::DatasetViewModel::GetDatasetRole).value<pcCoreDataset>());
}

//------------------------------------------------------------------------------

Datasets::Datasets(MainWin& mainWin_): super(mainWin_,"Datasets",Qt::Vertical) {
  box->addWidget((datasetView = new DatasetView(mainWin.session)));
  box->addWidget(textButton(mainWin.actImagesCombine));

  connect(&mainWin.session, &Session::fileSelected, [this](pcCoreFile) {
    datasetView->reset();
    datasetView->setCurrentIndex(mainWin.session.datasetViewModel.index(0,0)); // TODO untangle
  });
}

}

// eof

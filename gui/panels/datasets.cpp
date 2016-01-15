#include "datasets.h"
#include "mainwin.h"

namespace panel {

DatasetView::DatasetView(Session& session_): session(session_) {
  setModel(&session.datasetListModel);
}

void DatasetView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto& model  = session.datasetListModel;

  auto indexes = selected.indexes();
  model.session.setSelectedDataset(indexes.isEmpty()
                                   ? nullptr
                                   : model.data(indexes.first(), Session::DatasetListModel::GetFileRole).value<pcCoreDataset>());
}

//------------------------------------------------------------------------------

Datasets::Datasets(MainWin& mainWin_): super(mainWin_,"Datasets",Qt::Vertical) {
  box->addWidget((datasetView = new DatasetView(mainWin.session)));
  box->addWidget(textButton(mainWin.actImagesCombine));

  connect(&mainWin.session, &Session::fileSelected, [this](pcCoreFile) {
    datasetView->reset();
    datasetView->setCurrentIndex(mainWin.session.datasetListModel.index(0)); // TODO untangle
  });
}

}

// eof

#include "datasets.h"
#include "mainwin.h"

namespace panel {

DatasetList::DatasetList(Session& session_): session(session_) {
  setModel(&session.datasetListModel);
}

void DatasetList::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto& model  = session.datasetListModel;

  auto indexes = selected.indexes();
  model.session.setSelectedDataset(indexes.isEmpty()
                                   ? nullptr
                                   : model.data(indexes.first(), Session::DatasetListModel::GetFileRole).value<pcCoreDataset>());
}

//------------------------------------------------------------------------------

Datasets::Datasets(MainWin& mainWin): super(mainWin,"Datasets",Qt::Vertical) {
  box->addWidget((datasetList = new DatasetList(mainWin.session)));
  box->addWidget(textButton(mainWin.actImagesCombine));

  connect(&mainWin.session, &Session::fileSelected, this, [&](pcCoreFile) {
    datasetList->reset();
    datasetList->setCurrentIndex(mainWin.session.datasetListModel.index(0)); // TODO untangle
  });
}

}

// eof

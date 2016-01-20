#ifndef DATASETS_H
#define DATASETS_H

#include "panel.h"

namespace panel {

class DatasetView: public TreeListView {
  SUPER(DatasetView,TreeListView)
public:
  DatasetView(Session&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  Session &session;
};

class Datasets: public BoxPanel {
  SUPER(Datasets,BoxPanel)
public:
  Datasets(MainWin&,Session&);

private:
  DatasetView *datasetView;
};

}

#endif

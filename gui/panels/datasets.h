#ifndef DATASETS_H
#define DATASETS_H

#include "panel.h"

namespace panel {

class DatasetView: public ListView {
  SUPER(DatasetView,ListView)
public:
  DatasetView(Session&); // TODO Session will be broadcast, eventually

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  Session &session;
};

class Datasets: public BoxPanel {
  SUPER(Datasets,BoxPanel)
public:
  Datasets(MainWin&);

private:
  DatasetView *datasetView;
};

}

#endif

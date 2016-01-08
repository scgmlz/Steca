#ifndef DATASETS_H
#define DATASETS_H

#include "panel.h"

namespace panel {

class DatasetList: public ListView {
  SUPER(DatasetList,ListView)
public:
  DatasetList(Session&); // TODO Session will be broadcast, eventually

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  Session &session;
};

class Datasets: public Panel {
  SUPER(Datasets,Panel)
public:
  Datasets(MainWin&);

private:
  DatasetList *datasetList;
};

}

#endif

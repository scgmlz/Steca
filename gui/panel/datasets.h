/** \file
 */

#ifndef DATASETS_H
#define DATASETS_H

#include "panel.h"

namespace model {
class DatasetViewModel;
}

namespace panel {
//------------------------------------------------------------------------------

class DatasetView: public TreeListView {
  SUPER(DatasetView,TreeListView)
public:
  using Model = model::DatasetViewModel;

  DatasetView(Model&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  Model &model;
};

class Datasets: public BoxPanel {
  SUPER(Datasets,BoxPanel)
public:
  Datasets(MainWin&,Session&);

private:
  DatasetView *datasetView;
};

//------------------------------------------------------------------------------
}
#endif

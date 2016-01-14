#ifndef DATASETINFO_H
#define DATASETINFO_H

#include "panel.h"
#include <QTreeWidget>

namespace panel {

class DatasetInfo: public BoxPanel {
  SUPER(DatasetInfo,BoxPanel)
public:
  DatasetInfo(MainWin&);
};

}

#endif

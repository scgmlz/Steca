/** \file
 */

#ifndef DATASETINFO_H
#define DATASETINFO_H

#include "panel.h"

namespace panel {

class DatasetInfo: public BoxPanel {
  SUPER(DatasetInfo,BoxPanel)
public:
  DatasetInfo(MainWin&,Session&);

private:
  class Info: public QWidget {
    SUPER(Info,QWidget)
  public:
    Info(InfoItems&);
    QGridLayout *grid;
  };

  Info *info;
  InfoItems infoItems;

  void selectionChanged();
};

}

// used in signals

#endif

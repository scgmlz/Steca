#ifndef DATASETINFO_H
#define DATASETINFO_H

#include "panel.h"
#include <QTreeWidget>

class QCheckBox;
class QLabel;

namespace panel {

class DatasetInfo: public BoxPanel {
  SUPER(DatasetInfo,BoxPanel)
public:
  DatasetInfo(MainWin&);

  struct infoitem_t {
    str tag; QCheckBox *cb; QLabel *text;
  };
  typedef QVector<infoitem_t> InfoItems;

  class Info: public QWidget {
    SUPER(Info,QWidget)
  public:
    Info(InfoItems&);

  private:
    QGridLayout *grid;
  };

private:
  Info *info;
  InfoItems infoItems;
};

}

#endif

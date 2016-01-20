#ifndef DATASETINFO_H
#define DATASETINFO_H

#include "panel.h"
#include "core_dataset.h"
#include <QTreeWidget>

class QCheckBox;
class QLabel;

namespace panel {

class DatasetInfo: public BoxPanel {
  SUPER(DatasetInfo,BoxPanel)
public:
  DatasetInfo(MainWin&,Session&);

  struct infoitem_t {
    str tag; QCheckBox *cb; QLineEdit *text;
  };

  typedef QVector<infoitem_t> InfoItems;

  class Info: public QWidget {
    SUPER(Info,QWidget)
  public:
    Info(InfoItems&);

    QGridLayout *grid;
  private:
  };

private:
  Info *info;
  InfoItems infoItems;

  void selectionChanged();
  int numColumns;
};

}

// used in signals

#endif

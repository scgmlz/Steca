#include "datasetinfo.h"
#include <QScrollArea>

namespace panel {

DatasetInfo::DatasetInfo(MainWin& mainWin)
: super(mainWin,"Dataset Info",Qt::Vertical) {
  box->setMargin(0);
  auto scrollArea = new QScrollArea;
  box->addWidget(scrollArea);

  scrollArea->setFrameStyle(QFrame::NoFrame);

  infoitem_t item; item.tag = "A";
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);
  infoItems.append(item);

  info = new Info(infoItems);
  scrollArea->setWidget(info);
}

DatasetInfo::Info::Info(DatasetInfo::InfoItems& items) {
  setLayout((grid = gridLayout()));

  for (auto &item: items) {
    int row = grid->rowCount();
    grid->addWidget((item.cb   = check(item.tag)),  row, 0);
    grid->addWidget((item.text = label(str::null)), row, 1);
  }
}

}

// eof

#include "datasetinfo.h"
#include "mainwin.h"
#include "dataset.h"
#include <QScrollArea>

namespace panel {

DatasetInfo::DatasetInfo(MainWin& mainWin,Session& session)
: super("Dataset Info",mainWin,session,Qt::Vertical) {
  box->setMargin(0);
  auto scrollArea = new QScrollArea;
  box->addWidget(scrollArea);

  scrollArea->setFrameStyle(QFrame::NoFrame);

  for_i(core::Datasets::NUM_ATTRIBUTES) {
      infoitem_t item; item.tag = core::Datasets::getAttributeTag(i);
      infoItems.append(item);
  }

  info = new Info(infoItems);
  scrollArea->setWidget(info);

  connect(&session, &Session::datasetSelected, [this](pcCoreDataset dataset) {
    for_i(core::Datasets::NUM_ATTRIBUTES) {
      infoItems[i].text->setText(dataset ? dataset->getAttributeStrValue(i) : EMPTY_STR);
    }
  });

  for (auto &item: infoItems)
    connect(item.cb, &QCheckBox::clicked, this, &thisCls::selectionChanged);
}

void DatasetInfo::selectionChanged() {
  session.datasetViewModel.setInfoItems(&infoItems);
}

DatasetInfo::Info::Info(DatasetInfo::InfoItems& items) {
  setLayout((grid = gridLayout()));

  for (auto &item: items) {
    int row = grid->rowCount();
    grid->addWidget((item.cb   = check(item.tag)), row, 0);
    grid->addWidget((item.text = readCell(16)),    row, 1);
  }
}

}

// eof

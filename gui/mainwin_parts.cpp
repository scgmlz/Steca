#include "mainwin_parts.h"
#include "mainwin.h"
#include "panel/dataset.h"
#include "panel/fitting.h"
#include "panel/diffractogram.h"

namespace panel {
//-----------------------------------------------------------------------------

class FileViewDelegate: public QStyledItemDelegate {
  SUPER(FileViewDelegate,QStyledItemDelegate)
public:
  void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const {
    QStyleOptionViewItem o = option;
    bool isCorrectionFile = index.data(FileView::Model::IsCorrectionFileRole).toBool();
    if(isCorrectionFile) {
      o.font.setItalic(true);
      o.font.setBold(true);
    }
    super::paint(painter,o,index);
  }
};

//------------------------------------------------------------------------------

FileView::FileView(TheHub& theHub): model(theHub.fileViewModel) {
  setModel(&model);

  static FileViewDelegate delegate;
  setItemDelegate(&delegate);
}

void FileView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  model.setSelectedFile(indexes.isEmpty()
    ? core::shp_File()
    : model.data(indexes.first(), Model::GetFileRole).value<core::shp_File>());
}

void FileView::removeSelected() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  uint row = index.row();
  index = ((int)(row+1) < model.rowCount()) ? index : index.sibling(row-1,0);

  model.remFile(row);
  if (0>=model.rowCount()) // no more files
    model.setSelectedFile(core::shp_File());

  setCurrentIndex(index);
}

void FileView::update() {
  auto index = currentIndex();
  model.signalReset();
  // keep the current index, or select the first item
  setCurrentIndex(index.isValid() ? index : model.index(0));
}

//------------------------------------------------------------------------------

DockFiles::DockFiles(TheHub& theHub)
: super("Files","dock-files",Qt::Vertical) {
  box->addWidget((fileView = new FileView(theHub)));

  auto h = hbox(); box->addLayout(h);

  h->addWidget(textButton(theHub.actLoadCorrectionFile));
  h->addWidget(iconButton(theHub.actImagesEnableCorr));
  h->addStretch();
  h->addWidget(iconButton(theHub.actAddFiles));
  h->addWidget(iconButton(theHub.actRemoveFile));

  connect(theHub.actRemoveFile, &QAction::triggered, [this]() {
    fileView->removeSelected();
  });

  connect(&theHub, &TheHub::filesChanged, [this]() {
    fileView->update();
  });
}

//------------------------------------------------------------------------------

DatasetView::DatasetView(TheHub& theHub_): theHub(theHub_), model(theHub.datasetViewModel) {
  setModel(&model);

  connect(&theHub, &TheHub::fileSelected, [this](core::shp_File coreFile) {
    model.setCoreFile(coreFile);
    setCurrentIndex(model.index(0,0));
  });

  connect(&model, &QAbstractItemModel::modelReset, [this]() {
    for_i (model.columnCount())
      resizeColumnToContents(i);
  });
}

void DatasetView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  theHub.setSelectedDataset(indexes.isEmpty()
    ? core::shp_Dataset()
    : model.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Dataset>());
}

//------------------------------------------------------------------------------

DockDatasets::DockDatasets(TheHub& theHub)
: super("Datasets","dock-datasets",Qt::Vertical) {
  box->addWidget((datasetView = new DatasetView(theHub)));
  auto h = hbox();
  box->addLayout(h);
  h->addWidget(label("Combine:"));
  h->addWidget(spinCell(4,1));
}

//------------------------------------------------------------------------------

SplitImage::SplitImage(TheHub& theHub): super(Qt::Horizontal) {
  auto *options1 = new panel::DatasetOptions1(theHub);
  auto *options2 = new panel::DatasetOptions2(theHub);
  auto *dataset = new panel::Dataset(theHub);
  connect(options2, &panel::DatasetOptions2::imageScale, dataset, &panel::Dataset::setImageScale);
  box->addWidget(options1);
  box->addWidget(options2);
  box->addWidget(dataset);
  box->setStretch(2,1);
}

//------------------------------------------------------------------------------

SplitFitting::SplitFitting(TheHub& theHub): super(Qt::Vertical) {
  box->addWidget(new panel::Fitting(theHub));
}

//------------------------------------------------------------------------------

SplitDiffractogram::SplitDiffractogram(TheHub& theHub): super(Qt::Horizontal) {
  auto diffractogram = new panel::Diffractogram(theHub);
  diffractogram->setHorizontalStretch(1);
  box->addWidget(diffractogram);
}

//------------------------------------------------------------------------------

DockDatasetInfo::DockDatasetInfo(TheHub& theHub_)
: super("Dataset info","dock-dataset-info",Qt::Vertical), theHub(theHub_) {
  box->setMargin(0);
  auto scrollArea = new QScrollArea;
  box->addWidget(scrollArea);

  scrollArea->setFrameStyle(QFrame::NoFrame);

  for_i (core::Dataset::numAttributes()) {
      InfoItem item; item.tag = core::Dataset::getAttributeTag(i);
      infoItems.append(item);
  }

  info = new Info(infoItems);
  scrollArea->setWidget(info);

  for_i (core::Dataset::numAttributes()) {
      infoItems[i].cb->setToolTip("Show value in Datasets list");
  }

  connect(&theHub, &TheHub::datasetSelected, [this](core::shp_Dataset dataset) {
    for_i (core::Dataset::numAttributes()) {
      infoItems[i].text->setText(dataset ? dataset->getAttributeStrValue(i) : EMPTY_STR);
    }
  });

  for (auto &item: infoItems) {
    connect(item.cb, &QCheckBox::clicked, this, [this]() {
      theHub.datasetViewModel.setInfoItems(&infoItems);
    });
  }
}

DockDatasetInfo::Info::Info(InfoItems& items) {
  setLayout((grid = gridLayout()));

  for (auto &item: items) {
    int row = grid->rowCount();
    grid->addWidget((item.cb   = check(item.tag)), row, 0);
    grid->addWidget((item.text = readCell(16)),    row, 1);
  }
}

//------------------------------------------------------------------------------
}
// eof

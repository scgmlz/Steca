#include "mainwin_parts.h"
#include "mainwin.h"
#include "session.h"
#include "panel/dataset.h"
#include "panel/background.h"
#include "panel/reflections.h"
#include "panel/diffractogram.h"

namespace panel {
//-----------------------------------------------------------------------------

class FileViewDelegate: public QStyledItemDelegate {
  SUPER(FileViewDelegate,QStyledItemDelegate)
public:
  void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const {
    QStyleOptionViewItem o = option;
    bool isCorrectionFile = index.data(FileView::Model::IsCorrectionFileRole).toBool();
    if(isCorrectionFile)
      o.font.setItalic(true);
    super::paint(painter,o,index);
  }
};

//------------------------------------------------------------------------------

FileView::FileView(Model& model_): model(model_) {
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

void FileView::removeSelectedFile() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  uint row = index.row();
  index = (row+1 < model.numFiles(true)) ? index : index.sibling(row-1,0);
  model.remFile(row);
  if (0>=model.numFiles(true))
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

DockFiles::DockFiles(MainWin& mainWin,Session& session)
: super("Files","dock-files",Qt::Vertical) {
  box->addWidget((fileView = new FileView(session.fileViewModel)));

  auto h = hbox(); box->addLayout(h);

  h->addWidget(textButton(mainWin.actLoadCorrectionFile));
  h->addStretch();
  h->addWidget(iconButton(mainWin.actAddFiles));
  h->addWidget(iconButton(mainWin.actRemoveFile));

  connect(mainWin.actRemoveFile, &QAction::triggered, [this]() {
    fileView->removeSelectedFile();
  });

  connect(&session, &Session::filesChanged, [this]() {
    fileView->update();
  });
}

//------------------------------------------------------------------------------

DatasetView::DatasetView(Model& model_): model(model_) {
  setModel(&model);

  connect(&model.getSession(), &Session::fileSelected, [this](core::shp_File coreFile) {
    model.setCoreFile(coreFile);
    setCurrentIndex(model.index(0,0));
  });

  connect(&model, &QAbstractItemModel::modelReset, [this]() {
    for_i (model.columnCount(QModelIndex()))
      resizeColumnToContents(i);
  });
}

void DatasetView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  model.setSelectedDataset(indexes.isEmpty()
    ? core::shp_Dataset()
    : model.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Dataset>());
}

//------------------------------------------------------------------------------

DockDatasets::DockDatasets(MainWin&,Session& session)
: super("Datasets","dock-datasets",Qt::Vertical) {
  box->addWidget((datasetView = new DatasetView(session.datasetViewModel)));
  auto h = hbox();
  box->addLayout(h);
  h->addWidget(label("Combine:"));
  h->addWidget(spinCell(4,1));
}

//------------------------------------------------------------------------------

SplitImage::SplitImage(MainWin& mainWin,Session& session): super(Qt::Horizontal) {
  auto *options = new panel::DatasetOptions(mainWin,session);
  auto *dataset = new panel::Dataset(mainWin,session);
  connect(options, &panel::DatasetOptions::imageScale, dataset, &panel::Dataset::setImageScale);
  box->addWidget(options);
  box->addWidget(dataset);
  box->setStretch(1,1);
}

//------------------------------------------------------------------------------

SplitReflections::SplitReflections(MainWin& mainWin,Session& session): super(Qt::Vertical) {
  box->addWidget(new panel::Background(mainWin,session));
  box->addWidget(new panel::Reflections(mainWin,session));
}

//------------------------------------------------------------------------------

SplitDiffractogram::SplitDiffractogram(MainWin& mainWin,Session& session): super(Qt::Horizontal) {
  auto diffractogram = new panel::Diffractogram(mainWin,session);
  diffractogram->setHorizontalStretch(1);
  box->addWidget(diffractogram);
}

//------------------------------------------------------------------------------

DockDatasetInfo::DockDatasetInfo(MainWin&,Session& session)
: super("Dataset info","dock-dataset-info",Qt::Vertical) {
  box->setMargin(0);
  auto scrollArea = new QScrollArea;
  box->addWidget(scrollArea);

  scrollArea->setFrameStyle(QFrame::NoFrame);

  for_i (core::Dataset::NUM_ATTRIBUTES) {
      InfoItem item; item.tag = core::Dataset::getAttributeTag(i);
      infoItems.append(item);
  }

  info = new Info(infoItems);
  scrollArea->setWidget(info);

  connect(&session, &Session::datasetSelected, [this](core::shp_Dataset dataset) {
    for_i (core::Dataset::NUM_ATTRIBUTES) {
      infoItems[i].text->setText(dataset ? dataset->getAttributeStrValue(i) : EMPTY_STR);
    }
  });

  for (auto &item: infoItems)
    connect(item.cb, &QCheckBox::clicked, this, [this,&session]() {
      session.datasetViewModel.setInfoItems(&infoItems);
    });
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
